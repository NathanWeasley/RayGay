
#include <libguile.h>

#include "parser/parser.h"
#include "parser/wrapper.h"
#include "parser/converters.h"

#include "parser/pathfactory.h"
#include "parser/texturefactory.h"
#include "parser/materialfactory.h"
#include "parser/sceneobjectfactory.h"
#include "parser/lightsourcefactory.h"
#include "parser/camerafactory.h"
#include "parser/transformationfactory.h"
#include "parser/mathfactory.h"
#include "parser/schemefunctions.h"

#include "scene.h"
#include "renderersettings.h"

using namespace std;

char* VAR_SCENE = "__scene__";
char* VAR_CAMERA = "__camera__";
char* VAR_RENDERER = "__renderer__";
char* VAR_IMAGESIZE = "__image-size__";
char* VAR_BACKGROUND = "__background__";

// TODO: (set-settings) should call a function a set the settings immediately.

Parser::Parser() {
    scm_init_guile();
    init_wrapper_type();
    // Globals
    scm_c_define(VAR_SCENE, SCM_EOL);
    scm_c_define(VAR_CAMERA, SCM_EOL);
    scm_c_define(VAR_RENDERER, SCM_EOL);
    scm_c_define(VAR_IMAGESIZE, SCM_EOL);
    scm_c_define(VAR_BACKGROUND, SCM_EOL);

    scm_c_define_gsubr("set-settings",1,0,0,(SCM (*)()) Parser::set_settings);

    // My procedures
    PathFactory::register_procs();
    TextureFactory::register_procs();
    MaterialFactory::register_procs();
    SceneObjectFactory::register_procs();
    LightsourceFactory::register_procs();
    CameraFactory::register_procs();
    TransformationFactory::register_procs();
    MathFactory::register_procs();
    SchemeFunctions::register_procs();
}

void Parser::assignVariable(string var, double value) {
    scm_c_define(var.c_str(), scm_double2num(value));
}

void Parser::parse_expr(std::string expr) {
    gh_eval_str(expr.c_str());
}

void Parser::parse_file(std::string filename) {
    char original_working_dir[1024];

    // Change cwd to this files parent folder
    getcwd(original_working_dir,1024);
    string original_cwds = string(original_working_dir);
    string cwd = string(original_working_dir) + "/" + filename;
    string filename_clean = string(cwd);
    int idx = cwd.find_last_of('/');
    cwd.resize(idx);
    filename_clean = filename_clean.substr(idx+1, filename_clean.length());
    chdir(cwd.c_str());

    scm_c_primitive_load(filename_clean.c_str());

    chdir(original_working_dir);
}

SCM Parser::lookup(string var_name) {
	SCM var = scm_c_lookup(var_name.c_str());
	return scm_variable_ref(var);
}

void Parser::populate(Scene* scene, RendererSettings* renderersettings) {
    // Populate sceneobjects and lights
    SCM list = lookup(VAR_SCENE);
    if (SCM_FALSEP (scm_list_p (list))) {
	scm_error(NULL, "internal-populate-scene", "The variable 'scene' is not a list", SCM_UNSPECIFIED, NULL);
    }
    uint32_t length = scm_num2int(scm_length(list),0,"internal-populate-scene");

    //cout << "Scene objects: " << length << endl;

    for(uint32_t i = 0; i < length; i++) {
	SCM s_value = scm_list_ref(list, scm_int2num(i));
	//assert(!SCM_NFALSEP (scm_list_p (s_value)));
	if (isSceneObject(s_value)) {
	    //cout << "Found a scene object" << endl;
	    SceneObject* sceneobject = scm2sceneobject(s_value, "internal-populate-scene", 0);
	    scene->addObject(sceneobject);
	} else if (isLightsource(s_value)) {
	    //cout << "Found a lightsource" << endl;
	    Lightsource* light = scm2lightsource(s_value, "internal-populate-scene", 0);
	    scene->addLight(light);
	} else {
	    scm_error(NULL, "internal-populating-scene", "A non-sceneobject or non-lightsource found.", SCM_UNSPECIFIED, NULL);
	}
    }

    // Get renderer
    SCM s_renderer = lookup(VAR_RENDERER);
    RendererSettings::RendererType type;
    if (!SCM_NULLP(s_renderer)) {
	string r_string = scm2string(s_renderer);
	if (r_string == "raytracer") {
	    type = RendererSettings::RAYTRACER;
	} else if (r_string == "photonrenderer") {
	    type = RendererSettings::PHOTON_RENDERER;
	} else if (r_string == "pathtracer") {
	    type = RendererSettings::PATHTRACER;
	} else {
	    type = RendererSettings::NONE;
	    scm_error(NULL, "internal-setting-renderer", ("Unknown renderertype: " + r_string).c_str(), SCM_UNSPECIFIED, NULL);
	} 
    } else {
	type = RendererSettings::NONE;
    }
    renderersettings->renderertype = type;

    // Populate camera
    SCM s_camera = lookup(VAR_CAMERA);
    if (!SCM_NULLP(s_camera)) {
	Camera* camera = scm2camera(s_camera, "internal-get-camera", 0);
	scene->setCamera(camera);
    } else {
	if (renderersettings->renderertype != RendererSettings::NONE) {
	    cout << "No camera defined. Disabled rendering." << endl;
	    renderersettings->renderertype = RendererSettings::NONE;
	}
    }
    

    SCM s_image_size = lookup(VAR_IMAGESIZE);
    if (!SCM_NULLP(s_image_size)) {
	assert(SCM_NFALSEP (scm_list_p (s_image_size)));
	assert(scm_num2int(scm_length(s_image_size),0,"") == 2);
	SCM s_w = scm_list_ref(s_image_size, scm_int2num(0));
	uint32_t w = scm_num2int(s_w,0,"");
	SCM s_h = scm_list_ref(s_image_size, scm_int2num(1));
	uint32_t h = scm_num2int(s_h,0,"");
	renderersettings->image_width = w;
	renderersettings->image_height = h;
    }

    SCM s_background = lookup(VAR_BACKGROUND);
    if (!SCM_NULLP(s_background)) {
	char* subr = "internal: setting scene background";
	if (isWrappedObject(s_background)) {
	    Texture* texture = scm2texture(s_background, subr, 0);
	    scene->setBackground(texture);
	} else {
	    RGBA rgba = scm2rgba(s_background, subr, 0);
	    scene->setBackground(rgba);
	}
    }
}

// TODO: Set fog

SCM Parser::set_settings(SCM s_settings) 
{
    RendererSettings* renderersettings = RendererSettings::uniqueInstance();

    if (!SCM_NULLP(s_settings)) {
	if (SCM_FALSEP (scm_list_p (s_settings))) {
	    scm_error(NULL, "set-settings", "The settings is not a list", SCM_UNSPECIFIED, NULL);
	}
	uint32_t length = scm_num2int(scm_length(s_settings),0,"");

	assert(length % 2 == 0);
	uint32_t argc = length / 2;

	for(uint32_t i = 0; i < argc; i++) {
	    char* key_c = gh_symbol2newstr(scm_list_ref(s_settings, scm_int2num(i*2)),NULL);
	    string key = string(key_c);
	    SCM s_value = scm_list_ref(s_settings, scm_int2num(i*2+1));
	    if (key == "globalphotons") {
		uint32_t value = scm_num2int(s_value,0,"");
		renderersettings->global_photons_num = value;
	    } else if (key == "causticphotons") {
		double value = scm_num2double(s_value,0,"");
		renderersettings->caustic_photons_num = int(value);
	    } else if (key == "estimateradius") {
		double value = scm_num2double(s_value,0,"");
		renderersettings->estimate_radius = int(value);
	    } else if (key == "estimateradius") {
		double value = scm_num2double(s_value,0,"");
		renderersettings->estimate_radius = value;
	    } else if (key == "estimatesamples") {
		double value = scm_num2double(s_value,0,"");
		renderersettings->estimate_samples = int(value);
	    } else if (key == "finalgatherrays") {
		double value = scm_num2double(s_value,0,"");
		renderersettings->final_gather_rays = int(value);
	    } else if (key == "cachetolerance") {
		double value = scm_num2double(s_value,0,"");
		renderersettings->cache_tolerance = value;
	    } else if (key == "image-storage") {
		string s = scm2string(s_value);
		cout << "Setting " << s << endl;
		if (s == "memory") {
		    renderersettings->image_alloc_model = Allocator::MALLOC_ONLY;    
		} else if (s == "disc") {
		    renderersettings->image_alloc_model = Allocator::MMAP_ONLY;    
		} else if (s == "auto") {
		    renderersettings->image_alloc_model = Allocator::AUTO;    
		} else {
		    scm_error(NULL, "set-settings", ("Unknown image-storage model: " + s).c_str(), SCM_UNSPECIFIED, NULL);
		}
	    } else {
		cout << "Unknown setting: " << key << endl;
	    }
	}
    }
    return NULL;

}

