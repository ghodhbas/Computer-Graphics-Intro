#include "surf.h"
#include "extra.h"
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

	//rotate by 10 deg
	Matrix3f rot = Matrix3f::rotateY(0.0314f);

	int i = 0;

    // TODO: Here you should build the surface.  See surf.h for details.
	Curve c = profile;
	for (size_t j = 0; j < 200; j++)
	{	
		for (int k = 0 ; k< c.size(); k++)
		{	
			CurvePoint point = c[k];
			surface.VV.push_back(point.V);
			surface.VN.push_back(point.N);
			//save face
			if (i % 2 == 0 && i >= 2) surface.VF.push_back(Tup3u(i, i - 1, i - 2));
			i++;
			
			
		
			Vector3f vertex = rot  * point.V;
			Vector3f normal = rot *  point.N;
			// fix normals


			c[k].V = vertex;
			c[k].N = -normal;
			surface.VV.push_back(vertex);
			surface.VN.push_back(-normal);
			if (i % 2 == 1 && i >= 2) surface.VF.push_back(Tup3u(i, i - 2, i - 1));
			i++;

		}
	}
	


    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Matrix4f gencyl_transform_matrix(CurvePoint p)
{
	const Vector4f N(p.N, 0);
	const Vector4f B(p.B, 0);
	const Vector4f T(p.T, 0);
	const Vector4f V(p.V, 1);
	return Matrix4f(N, B, T, V);
}

Matrix3f gencyl_normal_transform_matrix(CurvePoint p)
{
	return gencyl_transform_matrix(p).getSubmatrix3x3(0, 0).inverse().transposed();
}

Surface makeGenCyl(const Curve& profile, const Curve& sweep)
{
	if (!checkFlat(profile))
	{
		cerr << "genCyl profile curve must be flat on xy plane." << endl;
		exit(0);
	}

	vector<Vector3f> VV;
	vector<Vector3f> VN;
	vector<Tup3u> VF;

	const int num_points = profile.size();
	// Duplicate profile for each point along the sweep curve.
	for (CurvePoint sweep_point : sweep)
	{
		vector<Vector3f> swept_Vs;
		vector<Vector3f> swept_Ns;
		for (CurvePoint p : profile)
		{
			swept_Vs.push_back(
				(gencyl_transform_matrix(sweep_point) * Vector4f(p.V, 1)).xyz()
			);
			// Invert the normals.
			swept_Ns.push_back(
				-1 * (gencyl_normal_transform_matrix(sweep_point) * p.N)
			);

		}
		if (VV.size() > 0)
		{
			// Link the previous profile copy to the newly generated one.
			for (int i = VV.size() - 1; i > VV.size() - num_points; --i)
			{
				// For each point in the previous profile copy, link it to
				// the other vertices in the triangle mesh.
				VF.push_back(Tup3u(i, i + num_points, i + num_points - 1));
				VF.push_back(Tup3u(i, i + num_points - 1, i - 1));
			}
		}
		for (Vector3f V : swept_Vs) VV.push_back(V);
		for (Vector3f N : swept_Ns) VN.push_back(N);
	}
	// Link the last copy to the initial profile curve.
	for (int i = 1; i < num_points; ++i)
	{
		VF.push_back(Tup3u(VV.size() - i, num_points - i, num_points - i - 1));
		VF.push_back(Tup3u(VV.size() - i, num_points - i - 1, VV.size() - i - 1));
	}
	// Sanity checks.
	return Surface{ VV, VN, VF };
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
