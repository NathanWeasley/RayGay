
#include <iostream>
#include <cassert>
#include <time.h>

#include "renderer.h"
#include "camera.h"
#include "image/image.h"
#include "scene.h"
#include "image/rgb.h"
#include "object.h"
#include "math/matrix.h"
#include "spacesubdivider.h"
#include "objectcollection.h"

Renderer::Renderer() {
}

/**
 * Render the scene into an image
 * TODO: Jitter the samples within the grid.
 * 
 * @param sc The scene to render
 * @param img The image to place pixels on
 * @param spc The space containing the objects of the scene
 */
void Renderer::render(Scene* sc, Image* img, SpaceSubdivider* spc) {
    time_t beginTime;
    scene = sc;
    space = spc;

    // Transform scene according to camera
    Camera* camera = scene->getCamera();
    /*
    Matrix orient = Matrix::matrixOrient(camera->getDirection(), camera->getUp());
    scene->transform(orient);
*/
    beginTime = time(NULL);
    // Add all objects in scene to spacesubdivider
    std::vector<object*> objects = scene->getObjects();
    for (vector<object*>::iterator p = objects.begin(); p != objects.end(); p++) {
	(*p)->prepare();
	space->addObject(*p);
    }
    //
    // Add all objectcollections in scene to spacesubdivider
    std::vector<ObjectCollection*> objectcols = scene->getObjectCollections();
    for (vector<ObjectCollection*>::iterator p = objectcols.begin(); p != objectcols.end(); p++) {
	(*p)->prepare();
	(*p)->addParts(space);
    }
    space->prepare();
    Stats::getUniqueInstance()->put("Prepare time (seconds)",time(NULL)-beginTime);

    aa_enabled = camera->isAAEnabled();
    aa_depth = camera->getAADepth();
    
    beginTime = time(NULL);
    int img_w = img->getWidth() / 2;
    int img_h = img->getHeight() / 2;
    int image_width = img_w*2;

    // Prepare the two PixelBlock buffers
    unsigned int block_size = 1 + (1 << aa_depth);
    if (aa_enabled) {
	cout << "Block size: " << block_size << endl;
	cout << "aa_depth: " << aa_depth << endl;
	row1.reserve(image_width);
	row2.reserve(image_width);
	for(int i = 0; i < image_width; i++) {
	    row1.push_back(PixelBlock(block_size));
	    row2.push_back(PixelBlock(block_size));
	}
    }

    std::vector<PixelBlock>* cur_row_ptr = &row1;
    std::vector<PixelBlock>* prev_row_ptr = &row2;
    std::vector<PixelBlock>* tmp_row_ptr;
    PixelBlock* cur_block;
    PixelBlock* prev_block;
    RGB color;
    for (int y = -img_h; y < img_h; y++) {
	if (y != -img_h) {
	    // Swap row buffers
	    tmp_row_ptr = cur_row_ptr;
	    cur_row_ptr = prev_row_ptr;
	    prev_row_ptr = tmp_row_ptr;
	    prepareCurRow(cur_row_ptr,prev_row_ptr,block_size);
	}
	for (int x = -img_w; x < img_w; x++) {
	    if (aa_enabled) {
		cur_block = &((*cur_row_ptr)[x + img_w]);
		if (x != -img_w) {
		    prev_block = &((*cur_row_ptr)[x + img_w - 1]);
		    prepareCurBlock(cur_block,prev_block,block_size);
		}
		color = getSubPixel(0, Vector2(x,y), cur_block, 1.0, 0, 0, block_size - 1, block_size - 1);
	    } else {
		color = getPixel(Vector2(x,y));
	    }
	    img->setRGB((int)x + img_w, (int)(-y) + img_h - 1, color);
	}
	cout << y + img_h << " / " << img_h*2 << "          \r" << flush;
    }
    Stats::getUniqueInstance()->put("Rendering time (seconds)",time(NULL)-beginTime);
}

/**
 * Clear cur_row and copy lowermost color values from prev_row into topmost color values in cur_row
 */
void Renderer::prepareCurRow(std::vector<PixelBlock>* cur_row, std::vector<PixelBlock>* prev_row,unsigned int blocksize) {
    assert(cur_row->size() == prev_row->size());
    PixelBlock* cur_row_block;
    PixelBlock* prev_row_block;
    unsigned int width = cur_row->size();

    for(unsigned int i = 0; i < width; i++) {
	cur_row_block = &((*cur_row)[i]);
	prev_row_block = &((*prev_row)[i]);
	cur_row_block->reset();
	
	for(unsigned int j = 0; j < blocksize; j++) {
	    if (prev_row_block->isActive(j,0)) {
		cur_row_block->setColor(j,blocksize-1,prev_row_block->getColor(j,0));
	    }
	}
    }
}

/**
 * Copies rightmost subpixels from prev_block into leftmost subpixels in cur_block
 */
void Renderer::prepareCurBlock(PixelBlock* cur_block, PixelBlock* prev_block, unsigned int blocksize) {
    for(unsigned int i = 0; i < blocksize; i++) {
	if (prev_block->isActive(blocksize-1,i)) {
	    cur_block->setColor(0,i,prev_block->getColor(blocksize-1,i));
	}
    }
}

RGB Renderer::getSubPixel(unsigned int curLevel, const Vector2& center, PixelBlock *block, double size, int x1, int y1, int x2, int y2) {

    double halfsize = size / 2.0;

    // Find corner pixels
    Vector2 lowerleft = center - Vector2(halfsize,halfsize);
    Vector2 upperright = center + Vector2(halfsize,halfsize);
    Vector2 upperleft = Vector2(lowerleft[0],upperright[1]);
    Vector2 lowerright = Vector2(upperright[0],lowerleft[1]);

    RGB c1,c2,c3,c4;
	
    // Trace upper left corner
    if (!block->isActive(x1,y1)) {
	c1 =  getPixel(upperleft);
	block->setColor(x1,y1,c1);
    } else {
	c1 = block->getColor(x1,y1);
    }

    // Trace lower left corner
    if (!block->isActive(x1,y2)) {
	c2 = getPixel(lowerleft);
	block->setColor(x1,y2,c2);
    } else {
	c2 = block->getColor(x1,y2);
    }

    // Trace upper right corner
    if (!block->isActive(x2,y1)) {
	c3 = getPixel(upperright);
	block->setColor(x2,y1,c3);
    } else {
	c3 = block->getColor(x2,y1);
    }
    
    // Trace lower right corner
    if (!block->isActive(x2,y2)) {
	c4 = getPixel(lowerright);
	block->setColor(x2,y2,c4);
    } else {
	c4 = block->getColor(x2,y2);
    }

    
    // Check if we need further supersampling
    // TODO: Vi burde sammenligne brightness og ikke afstande i RGB-rummet.
    if (aa_enabled && curLevel <= aa_depth) {
	if (c1.sqrDistance(c2) >= aa_threshhold ||
		c2.sqrDistance(c3) >= aa_threshhold ||
		c3.sqrDistance(c4) >= aa_threshhold ||
		c4.sqrDistance(c1) >= aa_threshhold ||
		c1.sqrDistance(c3) >= aa_threshhold ||
		c2.sqrDistance(c4) >= aa_threshhold) {

	    // Center of this sub-block
	    int xc = (x1 + x2) / 2;
	    int yc = (y1 + y2) / 2;

	    // Trace the four sub-blocks
	    c1 = getSubPixel(curLevel+1,(upperleft+center)*0.5,block,halfsize,x1,y1,xc,yc);
	    c2 = getSubPixel(curLevel+1,(lowerright+center)*0.5,block,halfsize,xc,yc,x2,y2);
	    c3 = getSubPixel(curLevel+1,(lowerleft+center)*0.5,block,halfsize,x1,yc,xc,y2);
	    c4 = getSubPixel(curLevel+1,(upperright+center)*0.5,block,halfsize,xc,y1,x2,yc);
	}
    }

    // Return average
    return 0.25 * (c1 + c2 + c3 + c4);
}

Renderer::PixelBlock::PixelBlock(const unsigned int size) {
    this->size = size;
    color = new RGB[size*size]; 
    active = new bool[size*size]; 
    reset();
}

void Renderer::PixelBlock::reset() {
    for(unsigned int i = 0; i < size*size; i++) {
	active[i] = false;
    }
}

