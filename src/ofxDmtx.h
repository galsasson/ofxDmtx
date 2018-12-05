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
	};
	typedef vector<Barcode> DecodeResult;
	static DecodeResult decode(ofPixels& pixels);
};


#endif /* TheApp_h */

