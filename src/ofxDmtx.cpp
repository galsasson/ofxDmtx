//
//  ofxDmtx.cpp
//  dmtx
//
//  Created by Gal Sasson on 12/05/18.
//
//

#include "ofxDmtx.h"

ofxDmtx::DecodeResult ofxDmtx::decode(ofPixels& pixels, float timeout, float edgeThresh, float squareDeviationAngle)
{
	DecodeResult res;
	DmtxImage      *img;
	DmtxDecode     *dec;
	DmtxRegion     *reg;
	DmtxMessage    *msg;
	
	img = dmtxImageCreate(pixels.getData(), pixels.getWidth(), pixels.getHeight(), DmtxPack24bppRGB);
	if (img==NULL) {
		return res;
	}
	
	dec = dmtxDecodeCreate(img, 1);
	if (dec==NULL) {
		dmtxImageDestroy(&img);
		return res;
	}
	
	dmtxDecodeSetProp(dec, DmtxPropEdgeThresh, edgeThresh);
	dmtxDecodeSetProp(dec, DmtxPropSquareDevn, squareDeviationAngle);

	while(true) {
		if (timeout==0) {
			reg = dmtxRegionFindNext(dec, NULL);
		}
		else {
			time_t sec = (int)floor(timeout);
			unsigned long msec = (unsigned long)((timeout-(int)timeout)*1000);
			DmtxTime endtime = dmtxTimeNow();
			endtime = dmtxTimeAdd(endtime, sec*1000+msec);
			reg = dmtxRegionFindNext(dec, &endtime);
		}
		
		if (reg == NULL) {
			// no more codes could be found
			break;
		}
		
		Barcode b;
		msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
		if (msg==NULL) {
			dmtxRegionDestroy(&reg);
			dmtxDecodeDestroy(&dec);
			dmtxImageDestroy(&img);
			return res;
		}
		
		b.arraySize = msg->arraySize;
		b.codeSize = msg->codeSize;
		b.outputSize = msg->outputSize;

#ifdef DEBUG
		int oned = sqrt(msg->arraySize);
		for (int i=0; i<msg->arraySize; i++){
			fprintf(stdout, " %c.", msg->array[i]);
			if (i%oned==oned-1){
				fprintf(stdout, "\n");
			}
		}
		fprintf(stdout, "\n\n");
		for (int j=0; j<msg->codeSize; j++){
			fprintf(stdout, " %c.", msg->code[j]);
		}
		fprintf(stdout, "\n\n");
		for (int k=0; k<msg->outputSize; k++){
			fprintf(stdout, " %c.", msg->output[k]);
		}
		fprintf(stdout, "\n\n");
		
		if(msg != NULL) {
			fputs("output: \"", stdout);
			fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
			fputs("\"\n", stdout);
			dmtxMessageDestroy(&msg);
		}
#endif
		// add message
		char* message = new char[msg->outputSize+1];
		strset(message, 0);
		strncpy(message, (const char*)(msg->output), msg->outputSize);
		b.output = string(message);
		delete message;
		
		// add corners
		DmtxVector2 tlCode={0,0}, trCode={1,0}, brCode={1,1}, blCode={0,1};
		DmtxVector2 tlImage,trImage,brImage,blImage;
		dmtxMatrix3VMultiply(&tlImage, &tlCode, reg->fit2raw);
		dmtxMatrix3VMultiply(&trImage, &trCode, reg->fit2raw);
		dmtxMatrix3VMultiply(&brImage, &brCode, reg->fit2raw);
		dmtxMatrix3VMultiply(&blImage, &blCode, reg->fit2raw);
		b.corners.push_back(ofDefaultVec2(tlImage.X, pixels.getHeight()-tlImage.Y));
		b.corners.push_back(ofDefaultVec2(trImage.X, pixels.getHeight()-trImage.Y));
		b.corners.push_back(ofDefaultVec2(brImage.X, pixels.getHeight()-brImage.Y));
		b.corners.push_back(ofDefaultVec2(blImage.X, pixels.getHeight()-blImage.Y));

		// add code transformation matrices
		b.fit2raw = ofMatrix3x3(reg->fit2raw[0][0], reg->fit2raw[1][0], reg->fit2raw[2][0],
								reg->fit2raw[0][1], reg->fit2raw[1][1], reg->fit2raw[2][1],
								reg->fit2raw[0][2], reg->fit2raw[1][2], reg->fit2raw[2][2]);
		b.raw2fit = ofMatrix3x3(reg->raw2fit[0][0], reg->raw2fit[1][0], reg->raw2fit[2][0],
								reg->raw2fit[0][1], reg->raw2fit[1][1], reg->raw2fit[2][1],
								reg->raw2fit[0][2], reg->raw2fit[1][2], reg->raw2fit[2][2]);

		res.push_back(b);
		dmtxRegionDestroy(&reg);
	}
	
	dmtxDecodeDestroy(&dec);
	dmtxImageDestroy(&img);
	return res;
}
