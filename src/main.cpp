#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <iostream>
#include <vector>

#include "stats.h"

#include "importer.h"

#include "math/vector.h"
#include "math/matrix.h"

#include "scene.h"
#include "image/image.h"
#include "space/kdtree.h"

#include "photonrenderer.h"
#include "raytracer.h"

#include "renderersettings.h"


using namespace std;

void work(string scenefile, string outputfile) {
    Stats::getUniqueInstance()->clear();
    //Stats::getUniqueInstance()->disable();

    cout << "Reading " << scenefile << endl;
    Importer importer(scenefile);
    Scene* scene = importer.getScene();
    cout << "Done." << endl;

    Vector2 img_size = importer.getImageSize();
    Image* img = new Image(int(img_size[0]),int(img_size[1]));

    Matrix n = Matrix::matrixRotate(Vector(1,1,0),21.0);
    //   n = n * Matrix::matrixTranslate(Vector(0,0,-500));
    //scene->transform(n);

    SpaceSubdivider* space = new KdTree();
    scene->initSpace(space);

    RendererSettings* renderersettings = importer.getRendererSettings();
    Renderer* renderer;

    if (renderersettings->renderertype == RendererSettings::PHOTON_RENDERER) {
	renderer = new PhotonRenderer(renderersettings,scene,space);
    } else if (renderersettings->renderertype == RendererSettings::RAYTRACER) {
	renderer = new Raytracer(renderersettings,scene,space);
    } else {
	cout << "main.cpp: Unknown renderer" << endl;
	exit(EXIT_FAILURE);
    }
    renderer->init();
    renderer->render(img);

    img->save(outputfile);
    delete img;
    Stats::getUniqueInstance()->dump();
}

int main(int argc, char *argv[]) {
    cout << "Raygay version 0.1" << endl;
    if (argc < 3) {
	cout << "USAGE: tracer <scenefile.ray> <outputfile.tga>" << endl;
	return EXIT_FAILURE;
    }
    srand(1); // Make sure rand is seeded consistenly.
    work(string(argv[1]),string(argv[2])); 
    return EXIT_SUCCESS;
}


