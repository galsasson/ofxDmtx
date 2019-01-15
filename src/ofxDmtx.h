//
//  ofxDmtx.h
//  dmtx
//
//  Created by Gal Sasson on 12/05/18.
//
//

#ifndef ofxDmtx_h
#define ofxDmtx_h

#include "ofMain.h"
#include "dmtx.h"

class ofxDmtx
{
public:
	struct Barcode {
		int arraySize;
		int codeSize;
		int outputSize;
		string output;
		vector<ofDefaultVec2> corners;
		ofMatrix3x3 fit2raw;
		ofMatrix3x3 raw2fit;
	};
	typedef vector<Barcode> DecodeResult;
	static DecodeResult decode(ofPixels& pixels, float timeout=0, float edgeThresh=10, float squareDeviationAngle=50);	// no timeout by default
};


#endif /* TheApp_h */

