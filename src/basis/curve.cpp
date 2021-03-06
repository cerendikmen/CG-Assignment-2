#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;
using namespace FW;

namespace {

// Approximately equal to.  We don't want to use == because of
// precision issues with floating point.
inline bool approx(const Vec3f& lhs, const Vec3f& rhs) {
	const float eps = 1e-8f;
	return (lhs - rhs).lenSqr() < eps;
}

// This is the core routine of the curve evaluation code.  Unlike
// evalBezier, this is only designed to work on 4 control points.
// Furthermore, it requires you to specify an initial binormal
// Binit, which is iteratively propagated throughout the curve as
// the curvepoints are generated.  Any other function that creates
// cubic splines can use this function by a corresponding change
// of basis.
Curve coreBezier(const Vec3f& p0,
				 const Vec3f& p1,
				 const Vec3f& p2,
				 const Vec3f& p3,
				 const Vec3f& Binit,
				 unsigned steps) {

	Curve R(steps + 1);

	// YOUR CODE HERE (R1): build the basis matrix and loop the given number of steps,
	// computing points on the spline

	Mat4f B;
	// ...

	for (unsigned i = 0; i <= steps; ++i) {
		float t = (float)i / (float)steps;
		float x, y, z;

		x = pow((1 - t), 3)*(p0.x) + 3 * t*(pow((1 - t), 2))*(p1.x) + 3 * t*t*(1 - t)*(p2.x) + t*t*t*(p3.x);
		y = pow((1 - t), 3)*(p0.y) + 3 * t*(pow((1 - t), 2))*(p1.y) + 3 * t*t*(1 - t)*(p2.y) + t*t*t*(p3.y);
		z = pow((1 - t), 3)*(p0.z) + 3 * t*(pow((1 - t), 2))*(p1.z) + 3 * t*t*(1 - t)*(p2.z) + t*t*t*(p3.z);
		R[i].V = Vec3f(x, y, z);

		x = -3 * (pow(1 - t, 2))*(p0.x) + (3 * (pow(1 - t, 2)) - 6 * t*(1 - t))* (p1.x) + (6 * t*(1 - t) - 3 * t*t)*(p2.x) + 3 * t*t*(p3.x);
		y = -3 * (pow(1 - t, 2))*(p0.y) + (3 * (pow(1 - t, 2)) - 6 * t*(1 - t))* (p1.y) + (6 * t*(1 - t) - 3 * t*t)*(p2.y) + 3 * t*t*(p3.y);
		z = -3 * (pow(1 - t, 2))*(p0.z) + (3 * (pow(1 - t, 2)) - 6 * t*(1 - t))* (p1.z) + (6 * t*(1 - t) - 3 * t*t)*(p2.z) + 3 * t*t*(p3.z);
		Vec3f tangent(x, y, z);
		tangent.normalize();
		R[i].T = tangent;

		x = 6 * (1 - t)*(p0.x) + (6 * t - 12 * (1 - t))*(p1.x) + (6 * (1 - t) - 12 * t)*(p2.x) + (6 * t*(p3.x));
		y = 6 * (1 - t)*(p0.y) + (6 * t - 12 * (1 - t))*(p1.y) + (6 * (1 - t) - 12 * t)*(p2.y) + (6 * t*(p3.y));
		z = 6 * (1 - t)*(p0.z) + (6 * t - 12 * (1 - t))*(p1.z) + (6 * (1 - t) - 12 * t)*(p2.z) + (6 * t*(p3.z));
		Vec3f normal(x, y, z);
		normal.normalize();
		R[i].N = normal;

		Vec3f binormal = cross(R[i].T, R[i].N);
		R[i].B = binormal;
	}

	return R;
}    

} // namespace
    
Curve evalBezier(const vector<Vec3f>& P, unsigned steps) {
	// Check
	if (P.size() < 4 || P.size() % 3 != 1) {
		cerr << "evalBezier must be called with 3n+1 control points." << endl;
		exit(0);
	}

	Curve C;

	for (int i = 0; i < P.size(); i += 4)
	{
		Curve Cx = coreBezier(P[i], P[i+1], P[i+2], P[i+3], Vec3f(0, 0, 0), steps);
		C.insert(C.begin(), Cx.begin(), Cx.end());
	}

    // YOUR CODE HERE (R1):
    // You should implement this function so that it returns a Curve
    // (e.g., a vector<CurvePoint>).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

	// EXTRA CREDIT NOTE:
    // Also compute the other Vec3fs for each CurvePoint: T, N, B.
    // A matrix [N, B, T] should be unit and orthogonal.
    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity. The T, N and B vectors will not
	// have to be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector<Vec3f>): "<< endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

    // Right now this will just return this empty curve.
    return C;
}

Curve evalBspline(const vector<Vec3f>& P, unsigned steps) {
    // Check
    if (P.size() < 4) {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit(0);
    }

    // YOUR CODE HERE (R2):
    // We suggest you implement this function via a change of basis from
	// B-spline to Bezier.  That way, you can just call your evalBezier function.
	Mat4f basis;
	/*basis.setRow(0, Vec4f(1,-3,3,-1));
	basis.setRow(1, Vec4f(4,0,-6,3));
	basis.setRow(2, Vec4f(1,3,3,-3));
	basis.setRow(3, Vec4f(0,0,0,1));
	basis *= 1 / 6;*/
	basis.setRow(0, Vec4f((float)1 / 6, (float)-1 / 2, (float)1 / 2, (float)-1 / 6));
	basis.setRow(1, Vec4f((float)2 / 3, 0, (float)-1, (float)1 / 2));
	basis.setRow(2, Vec4f((float)1 / 6, (float)1 / 2, (float)1 / 2, (float)-1 / 2));
	basis.setRow(3, Vec4f(0, 0, 0, (float)1 / 6));
	Curve C;
	for (int j = 0; j + 3 < P.size(); j++)
	{
		Curve Cx(steps+1);
		for (int i = 0; i <= steps; i++)
		{
			float t = (float)i / (float)steps;
			
			Mat4f mat;
			mat.setCol(0, Vec4f(P[j], 0));
			mat.setCol(1, Vec4f(P[j+1], 0));
			mat.setCol(2, Vec4f(P[j+2], 0));
			mat.setCol(3, Vec4f(P[j+3], 0));
			Vec4f tes(1, t, t*t, t*t*t);

			Mat4f final = mat * basis;
			Vec4f final2 = mat* basis * tes;

			Cx[i].V = final2.getXYZ();
			
		}
		C.insert(C.begin(), Cx.begin(), Cx.end());
	}
    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vec3f >): "<< endl;
    for (unsigned i = 0; i < P.size(); ++i) {
        cerr << "\t>>> "; printTranspose(P[i]); cerr << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;
    cerr << "\t>>> Returning empty curve." << endl;

    // Return an empty curve right now.
    return C;
}

Curve evalCircle(float radius, unsigned steps) {
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector<CurvePoint>).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R(steps+1);

    // Fill it in counterclockwise
    for (unsigned i = 0; i <= steps; ++i) {
        // step from 0 to 2pi
        float t = 2.0f * (float)M_PI * float(i) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vec3f(FW::cos(t), FW::sin(t), 0);
        
        // Tangent vector is first derivative
        R[i].T = Vec3f(-FW::sin(t), FW::cos(t), 0);
        
        // Normal vector is second derivative
        R[i].N = Vec3f(-FW::cos(t), -FW::sin(t), 0);

        // Finally, binormal is facing up.
        R[i].B = Vec3f(0, 0, 1);
    }

    return R;
}

void drawCurve(const Curve& curve, float framesize) {
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Setup for line drawing
    glDisable(GL_LIGHTING); 
    glColor4f(1, 1, 1, 1);
    glLineWidth(1);
    
	if (framesize >= 0) {
		// Draw curve
		glBegin(GL_LINE_STRIP);
		for (unsigned i = 0; i < curve.size(); ++i) {
			glVertex(curve[i].V);
		}
		glEnd();
	}

    glLineWidth(1);

    // Draw coordinate frames if framesize nonzero
    if (framesize != 0.0f) {
		framesize = FW::abs(framesize);
        Mat4f M;

        for (unsigned i = 0; i < curve.size(); ++i) {
            M.setCol( 0, Vec4f( curve[i].N, 0 ) );
            M.setCol( 1, Vec4f( curve[i].B, 0 ) );
            M.setCol( 2, Vec4f( curve[i].T, 0 ) );
            M.setCol( 3, Vec4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf(M.getPtr());
            glScaled(framesize, framesize, framesize);
            glBegin(GL_LINES);
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

