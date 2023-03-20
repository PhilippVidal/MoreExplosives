#ifndef HDSN_MATH 
	#define HDSN_MATH
class HDSN_Math
{
	
	static float AngleBetweenVectorsRAD(vector u, vector v)
	{
		return Math.Acos(vector.Dot(u, v));
	}
	
	static float AngleBetweenVectorsDEG(vector u, vector v)
	{
		return AngleBetweenVectorsRAD(u, v) * Math.RAD2DEG; 
	}
	
	static float SignedAngleBetweenVectorsRAD(vector u, vector v, vector n)
	{
		float angle = AngleBetweenVectorsRAD(u, v);
		if(vector.Dot(u * v, n) < 0)
		{
			return -angle;
		}	
		return angle;
	}
	
	static float SignedAngleBetweenVectorsDEG(vector u, vector v, vector n)
	{
		return SignedAngleBetweenVectorsRAD(u, v, n) * Math.RAD2DEG; 
	}
	
	static void TransposeMatrix3(vector in[3], out vector outMat[3])
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				outMat[i][j] = in[j][i];
			}
		}
	}

	static void RotationAroundAxis(vector axis, float angle, out vector rotMat[3])
	{
		float sin = Math.Sin(angle * Math.DEG2RAD); 
		float cos = Math.Cos(angle * Math.DEG2RAD);
		
		float x = axis[0];
		float y = axis[1];
		float z = axis[2];
		
		float oneMinusCos = (1 - cos);
		
		float a = x * oneMinusCos;
		float b = y * oneMinusCos;
		float c = z * oneMinusCos;

		float d = y * a;
		float e = x * c;
		float f = z * b;

		rotMat[0][0] = cos + x * a;
		rotMat[0][1] = z * sin + d;
		rotMat[0][2] = -y * sin + e;
		
		rotMat[1][0] = d - z * sin;
		rotMat[1][1] = cos + y * b;
		rotMat[1][2] = x * sin + f;
		
		rotMat[2][0] = y * sin + e;
		rotMat[2][1] = -x * sin + f;
		rotMat[2][2] = cos + z * c;	
	}
	
	static vector ProjectOnPlane(vector vec, vector planeNormal)
	{
		return vec - vector.Dot(vec, planeNormal) * planeNormal);
	}
}
#endif