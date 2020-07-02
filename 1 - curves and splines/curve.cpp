#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        exit( 0 );
    }

	Matrix4f spline_base(1 / 6.f, -3 / 6.f, 3 / 6.f, -1 / 6.f,
		4 / 6.f, 0.f, -6 / 6.f, 3 / 6.f,
		1 / 6.f, 3 / 6.f, 3 / 6.f, -3 / 6.f,
		0.f, 0.f, 0.f, 1 / 6.f);
	
	Matrix4f inv_spline_base(1.f, 1.f, 1.f, 1.f,
			- 1.f, 0.f, 1.f, 2.f,
			2 / 3.f, -1 / 3.f, 2 / 3.f, 11 / 3.f,
			0.f, 0.f, 0.f, 6.f);

	Matrix4f inv_bezier_base(1.f, 1.f, 1.f, 1.f,
		0.f, 1 / 3.f, 2 / 3.f, 1.f,
		0.f, 0.f, 1 / 3.f, 1.f,
		0.f, 0.f, 0.f, 1.f);

	Matrix4f bezier_base(1.f , -3.f, 3.f, -1.f,
		0.f , 3.f, -6.f, 3.f ,
		0.f, 0.f, 3.f, -3.f,
		0.f, 0.f, 0.f, 1.f);


	// get number of vertices used for the entire curve
	// how many sections we have
	int nb_c = max(0, P.size() - 3);
	//alocate steps+1 * nb of sections
	Curve bspline_curve(max(steps + 1, steps * nb_c + 1));

	float time_step = 1 / ((float)steps);

	int ID = 0;
	for (int point_index = 3; point_index < P.size(); point_index++) {
		//get four control points
		Matrix4f g(Vector4f(P[point_index - 3], 0.f), Vector4f(P[point_index - 2], 0.f), Vector4f(P[point_index - 1], 0.f), Vector4f(P[point_index], 0.f));
		g = g * bezier_base * inv_spline_base;

		//initial binormal
		Vector3f b(1, 2, 1);

		for (int step_i = 0; step_i <= steps; step_i++) {

			float time = time_step * step_i;
			//vertex power base
			Vector4f power_base(1, time, time * time, time * time * time);
			//tangant power_base
			Vector4f t_power_base(0, 1, 2 * time, 3 * time * time);
			//calculate spline base * powe_base
			Vector4f t_base = spline_base * t_power_base;

			Vector4f V = g * spline_base * power_base;
			Vector4f T = g * t_base;
			T = T.normalized();
			Vector4f N;
			if (step_i == 0) {
				N = Vector4f(b) * T;
			}
			else {
				N = Vector4f(bspline_curve[step_i - 1].B) * T;
			}

			N = N.normalized();
			Vector4f B = (T * N).normalized();

			bspline_curve[step_i + ID].V = V.xyz();
			bspline_curve[step_i + ID].T = T.xyz();
			bspline_curve[step_i + ID].B = B.xyz();
			bspline_curve[step_i + ID].N = N.xyz();

		}
		ID += steps;
	}
	return bspline_curve;
}

Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

	Matrix4f spline_base(1/6.f, -3/6.f, 3/6.f, -1/6.f,
						 4/6.f, 0.f, -6/6.f, 3/6.f,
						 1/6.f, 3/6.f, 3/6.f, -3/6.f,
						 0.f, 0.f, 0.f, 1/6.f);

	Matrix4f inv_bezier_base(1.f, 1.f, 1.f, 1.f,
							 0.f, 1/3.f, 2/3.f, 1.f,
							 0.f, 0.f, 1/3.f, 1.f,
							 0.f, 0.f, 0.f, 1.f);

	// get number of vertices used for the entire curve
	// how many sections we have
	int nb_c = max(0, P.size() - 3);
	//alocate steps+1 * nb of sections
	Curve bspline_curve(max(steps+1, steps * nb_c +1 ));

	float time_step = 1 / ((float)steps);

	int ID = 0;
	for (int point_index = 3; point_index < P.size(); point_index++) {
		//get four control points
		Matrix4f g( Vector4f(P[point_index-3], 0.f) , Vector4f(P[point_index - 2], 0.f), Vector4f(P[point_index - 1], 0.f), Vector4f(P[point_index], 0.f));
		//initial binormal
		Vector3f b(1, 2, 1);

		for (int step_i = 0; step_i <= steps; step_i++) {

			float time = time_step * step_i;
			//vertex power base
			Vector4f power_base(1, time, time * time, time * time * time);
			//tangant power_base
			Vector4f t_power_base(0, 1, 2 * time, 3 * time * time);
			//calculate spline base * powe_base
			Vector4f t_base = spline_base * t_power_base;

			Vector4f V = g * spline_base * power_base;
			Vector4f T = g * t_base;
			T = T.normalized();
			Vector4f N;
			if (step_i == 0) {
				N = Vector4f(b) * T;
			}
			else {
				N = Vector4f(bspline_curve[step_i - 1].B) * T;
			}

			N = N.normalized();
			Vector4f B = (T * N).normalized();

			bspline_curve[step_i + ID].V = V.xyz();
			bspline_curve[step_i + ID].T = T.xyz();
			bspline_curve[step_i + ID].B = B.xyz();
			bspline_curve[step_i + ID].N = N.xyz();

		}
		ID += steps;
	}
	return bspline_curve;
}

Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
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

