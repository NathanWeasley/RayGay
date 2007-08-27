
#include "scheme/scheme.h"
#include "parser/imagefactory.h"
#include "parser/converters.h"
#include "parser/wrapper.h"
#include "image/image.h"
#include "image/imageimpl.h"
#include "imagefilters/gaussianblur.h"
#include "ttf.h"

SchemeObject* ImageFactory::modulate_mode_symbol;
SchemeObject* ImageFactory::add_mode_symbol;
SchemeObject* ImageFactory::replace_mode_symbol;
SchemeObject* ImageFactory::decal_mode_symbol;
ImageDrawing::AlphaCombineMode ImageFactory::alpha_combine_mode;

SchemeObject* ImageFactory::make_image(SchemeObject* s_width, SchemeObject* s_height, SchemeObject* s_background_color) {
    char* proc = "make-image";        
    int w = safe_scm2int(s_width, 1, proc);
    int h = safe_scm2int(s_height, 2, proc);
    Image* image = new ImageImpl<uint8_t,4>(w, h);
    
    if (s_background_color != S_UNSPECIFIED) {
        RGBA color = scm2rgba(s_background_color, proc, 3);
        image->clear(color);
    }
    
    return image2scm(image);
}

SchemeObject* ImageFactory::image_copy(SchemeObject* s_image) {
    char* proc = "image-copy";        
    Image* image = scm2image(s_image, proc, 1);
    Image* copy = new ImageImpl<uint8_t,4>(image->getWidth(), image->getHeight());
    copy->copy(*image);
    return image2scm(copy);
}

SchemeObject* ImageFactory::load_image(SchemeObject* s_filename) {
    string filename = scm2string(s_filename);        
    Image* image = Image::load(filename);
    return image2scm(image);
}

SchemeObject* ImageFactory::save_image(SchemeObject* s_image, SchemeObject* s_filename) {
    char* proc = "save-image";
    Image* image = scm2image(s_image, proc, 1);
    string filename = scm2string(s_filename);        
    image->save(filename);
    return S_UNSPECIFIED;
}

SchemeObject* ImageFactory::image_width(SchemeObject* s_image) {
    char* proc = "image-width";
    Image* image = scm2image(s_image, proc, 1);
    return int2scm(image->getWidth());
}

SchemeObject* ImageFactory::image_height(SchemeObject* s_image) {
    char* proc = "image-height";
    Image* image = scm2image(s_image, proc, 1);
    return int2scm(image->getHeight());
}

SchemeObject* ImageFactory::set_pixel(SchemeObject* s_image, SchemeObject* s_x, SchemeObject* s_y, SchemeObject* s_color) {
    char* proc = "set-pixel";
    Image* image = scm2image(s_image, proc, 1);
    double x = safe_scm2double(s_x, 2, proc);
    double y = safe_scm2double(s_y, 3, proc);
    RGBA color = scm2rgba(s_color, proc, 4);
    ImageDrawing::pixel(image, x, y, color, alpha_combine_mode);
    return S_UNSPECIFIED;
}

SchemeObject* ImageFactory::get_pixel(SchemeObject* s_image, SchemeObject* s_x, SchemeObject* s_y) {
    char* proc = "get-pixel";
    Image* image = scm2image(s_image, proc, 1);
    double x = safe_scm2double(s_x, 2, proc);
    double y = safe_scm2double(s_y, 3, proc);
    RGB pixel = image->getRGBA(int(x),int(y));
    return rgb2scm(pixel);
}

SchemeObject* ImageFactory::draw_line(SchemeObject* s_image, SchemeObject* s_from, SchemeObject* s_to, SchemeObject* s_color) {
    char* proc = "draw-line";
    Image* image = scm2image(s_image, proc, 1);
    double x0 = safe_scm2double(i_vector_ref(s_from, 0), 2, proc);
    double y0 = safe_scm2double(i_vector_ref(s_from, 1), 2, proc);
    double x1 = safe_scm2double(i_vector_ref(s_to, 0), 3, proc);
    double y1 = safe_scm2double(i_vector_ref(s_to, 1), 3, proc);
    RGBA color = scm2rgba(s_color, proc, 4);
    ImageDrawing::line(image, int(x0), int(y0), int(x1), int(y1), color, alpha_combine_mode);
    return S_UNSPECIFIED;
}

SchemeObject* ImageFactory::draw_circle(SchemeObject* s_image, SchemeObject* s_center, SchemeObject* s_radius, SchemeObject* s_color) {
    char* proc = "draw-line";
    Image* image = scm2image(s_image, proc, 1);
    // TODO: Check that s_center is a vector and that s_color is a color and that image is an image
    double x0 = safe_scm2double(i_vector_ref(s_center, 0), 2, proc);
    double y0 = safe_scm2double(i_vector_ref(s_center, 1), 2, proc);
    double r = safe_scm2double(s_radius, 3, proc);
    RGBA color = scm2rgba(s_color, proc, 4);
    ImageDrawing::circle(image, int(x0), int(y0), int(r), color, alpha_combine_mode);
    return S_UNSPECIFIED;
}

SchemeObject* ImageFactory::draw_string(SchemeObject* s_image, SchemeObject* s_pos, SchemeObject* s_text, SchemeObject* s_size, SchemeObject* s_ttf_file, SchemeObject* s_color) {
    char* proc = "draw-string";
    Image* image = scm2image(s_image, proc, 1);
    double x0 = safe_scm2double(i_vector_ref(s_pos, 0), 2, proc);
    double y0 = safe_scm2double(i_vector_ref(s_pos, 1), 2, proc);
    wstring str = scm2wstring(s_text);
    string ttf_filename = scm2string(s_ttf_file);
    double size = safe_scm2double(s_size, 4, proc);
    RGBA color = scm2rgba(s_color, proc, 6);
    
    TrueTypeFont* font = new TrueTypeFont(ttf_filename);
    ImageDrawing::string(image, int(x0), int(y0), str, font, size, color, alpha_combine_mode);
    return S_UNSPECIFIED;
}

SchemeObject* ImageFactory::apply_gaussian_blur(SchemeObject* s_image, SchemeObject* s_radius) {
    char* proc = "apply-gaussian-blur";
    Image* image = scm2image(s_image, proc, 1);
    double r = safe_scm2double(s_radius, 2, proc);
    // TODO: Check that r is positive
    GaussianBlur filter = GaussianBlur(r);
    filter.apply(image);        
    return S_UNSPECIFIED;
}

SchemeObject* ImageFactory::set_alpha_combine_mode(SchemeObject* s_mode) {
    char* proc = "set-alpha-combine-mode";         

    assert_arg_type(proc, 1, s_symbol_p, s_mode);        

    if (s_mode == decal_mode_symbol) {
        alpha_combine_mode = ImageDrawing::DECAL;    
    } else if (s_mode == replace_mode_symbol) {
        alpha_combine_mode = ImageDrawing::REPLACE;    
    } else if (s_mode == modulate_mode_symbol) {
        alpha_combine_mode = ImageDrawing::MODULATE;    
    } else if (s_mode == add_mode_symbol) {
        alpha_combine_mode = ImageDrawing::ADD;    
    } else {
        throw scheme_exception(proc, "Unknown alpha combine mode. Use 'decal, 'replace, 'add or 'modulate.");      
    }        
    return S_UNSPECIFIED;
}


void ImageFactory::register_procs(Scheme* scheme) {
    scheme->assign("make-image",2,1,0,(SchemeObject* (*)()) ImageFactory::make_image);
    scheme->assign("image-copy",1,0,0,(SchemeObject* (*)()) ImageFactory::image_copy);
    scheme->assign("load-image",1,0,0,(SchemeObject* (*)()) ImageFactory::load_image);
    scheme->assign("save-image",2,0,0,(SchemeObject* (*)()) ImageFactory::save_image);
    scheme->assign("set-pixel",4,0,0,(SchemeObject* (*)()) ImageFactory::set_pixel);
    scheme->assign("get-pixel",3,0,0,(SchemeObject* (*)()) ImageFactory::get_pixel);
    scheme->assign("draw-line",4,0,0,(SchemeObject* (*)()) ImageFactory::draw_line);
    scheme->assign("draw-circle",4,0,0,(SchemeObject* (*)()) ImageFactory::draw_circle);
    scheme->assign("draw-string",6,0,0,(SchemeObject* (*)()) ImageFactory::draw_string);
    scheme->assign("image-width",1,0,0,(SchemeObject* (*)()) ImageFactory::image_width);
    scheme->assign("image-height",1,0,0,(SchemeObject* (*)()) ImageFactory::image_height);
    scheme->assign("set-alpha-combine-mode",1,0,0,(SchemeObject* (*)()) ImageFactory::set_alpha_combine_mode);
    scheme->assign("apply-gaussian-blur",2,0,0,(SchemeObject* (*)()) ImageFactory::apply_gaussian_blur);
    
    add_mode_symbol = SchemeObject::createSymbol("add");
    replace_mode_symbol = SchemeObject::createSymbol("replace");
    modulate_mode_symbol = SchemeObject::createSymbol("modulate");
    decal_mode_symbol = SchemeObject::createSymbol("decal");
    
    alpha_combine_mode = ImageDrawing::REPLACE;
}
 