//
// Created by aliev on 08/11/2023.
//

#ifndef ADA_A3_IMAGE_CONV_IMAGE_H
#define ADA_A3_IMAGE_CONV_IMAGE_H

/*
 * The idea is like that: to have an image on which the processing is done.
 * This image may be broken in regions of size NxN. Then each renderer would process the region and when it is done,
 * it would emmit SDL_UserEvent that contains coordinates of a region. In the best case, application would rerender only
 * the region, but I can settle on the rerendering of the whole image. Then, immediately, it should start
 */
class Image {

};


#endif //ADA_A3_IMAGE_CONV_IMAGE_H
