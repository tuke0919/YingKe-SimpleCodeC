//
// Created by KE TU on 2020-07-04.
//

#ifndef YINGKE_SIMPLECODEC_VIDEO_DECODE_H
#define YINGKE_SIMPLECODEC_VIDEO_DECODE_H

class VideoDecoder{

public:
    virtual int initDecoder(const char* video_path) = 0;
    virtual int decodeFile(const char* yuv_path) = 0;
};

#endif //YINGKE_SIMPLECODEC_VIDEO_DECODE_H
