#ifndef OFVERTEXSTARITERATOR3D_H
#define OFVERTEXSTARITERATOR3D_H
#include "ofMacros.h"
#include "ofMesh.h"
#include "ofSing.h"
#include "ofIterator.h"
#include "ofList.h"
namespace of
{
#define SCELL typename ofVertexStarIterator3D<_Traits>::sCell
#define IDS typename ofVertexStarIterator3D<_Traits>::ids
template <class _Traits> class ofVertexStarIterator3D : public ofIterator<_Traits>
{
public:
	
	typedef typename _Traits::sCell 	sCell;
	typedef typename _Traits::sVertex 	sVertex;
	typedef typename _Traits::ids 		ids;
	typedef ofSing<_Traits>				sSing;
	typedef ofMesh<_Traits>				sMesh;
	ofVertexStarIterator3D(sMesh *_mesh);
	
	~ofVertexStarIterator3D();
	
	bool initialize(ids init);
	bool finish();
	bool notFinish();
	
	bool operator++();
	sCell *operator->();
	sCell *operator*();
	ids operator&();
	
private:
	ids iatu, iinit, iv;
	sCell *atu;
	sVertex *v;
	
	ofList<ids> lista, flags;
	
	int status, is;
	// 0 - iterator sem posicao
	// 1 - iterator em posicao normal
	// 2 - iterator no limite inferior
	// 3 - iterator no limite superior
	void addNeighbor();
};
template <class _Traits> ofVertexStarIterator3D<_Traits>::ofVertexStarIterator3D(sMesh *_mesh)
: ofIterator<_Traits>(_mesh)
{
	is = iatu = iinit = iv = -1;
	status = 0;
	v = NULL;
}
template <class _Traits> ofVertexStarIterator3D<_Traits>::~ofVertexStarIterator3D()
{
}
template <class _Traits> bool ofVertexStarIterator3D<_Traits>::initialize(ids init)
{
	OF_ASSERT(init >= 0);
	iv = init;
	v = this->mesh->getVertex(iv);
	status = 1;
	
	OF_ASSERT(v->inMesh());
	
	iatu = iinit = -1;
	
	atu = NULL;
	is = -1;
	
	lista.clear();
	flags.clear();
	
	return ++(*this);
}
template<class _Traits> bool ofVertexStarIterator3D<_Traits>::operator++()
{
	if(status == 1)
	{
		if(iatu < 0)
		{
			if(is < 0)
			{
				is = 0;
				iatu = v->getSingCell(is);
				
				OF_ASSERT(iatu >=0);
				
				atu = this->mesh->getCell(iatu);
				OF_ASSERT( (atu) && (atu->inMesh()) );
				
				flags.insert(iatu);
			
				addNeighbor();
				
				iinit = iatu;
			}
		}
		else
		{
			OF_ASSERT( atu->getVertexIndex(iv) >= 0);
			
			if(lista.empty())
			{
				is++;
				iatu = v->getSingCell(is);
				
				if((iatu >= 0)&&(!flags.inList(iatu)))  // changed from (iatu>=0)
				{
					OF_ASSERT(!flags.inList(iatu));
					lista.insert(iatu);
					flags.insert(iatu);
				}
				else
				{
					atu = NULL;
					iatu = -1;
					status = 3;
					return false;
				}				
			}
			
			iatu  = lista.pos(0);
			lista.erase(0);
				
			atu = this->mesh->getCell(iatu);
			OF_ASSERT( (atu) && (atu->inMesh()) );
			addNeighbor();
		}
		
		return true;
	}
	else
		return false;
}
template<class _Traits> void ofVertexStarIterator3D<_Traits>::addNeighbor()
{
	ids temp;
	
	temp = atu->getMateId((atu->getVertexIndex(iv)+1)%4);
	if((temp >= 0)&&(!flags.inList(temp)))
	{
		flags.insert(temp);
		lista.insert(temp);
	}
	temp = atu->getMateId((atu->getVertexIndex(iv)+2)%4);
	if((temp >= 0)&&(!flags.inList(temp)))
	{
		flags.insert(temp);
		lista.insert(temp);
	}
	temp = atu->getMateId((atu->getVertexIndex(iv)+3)%4);
	if((temp >= 0)&&(!flags.inList(temp)))
	{
		flags.insert(temp);
		lista.insert(temp);
	}
	
	
	OF_ASSERT( atu->getVertexIndex(iv) >= 0);
}
template<class _Traits> SCELL* ofVertexStarIterator3D<_Traits>::operator->()
{
	return atu;
}
template<class _Traits> SCELL* ofVertexStarIterator3D<_Traits>::operator*()
{
	return atu;
}
template<class _Traits> IDS ofVertexStarIterator3D<_Traits>::operator&()
{
	return iatu;
}
template<class _Traits> bool ofVertexStarIterator3D<_Traits>::finish()
{
	return ((status == 2)||(status == 3));
}
template<class _Traits> bool ofVertexStarIterator3D<_Traits>::notFinish()
{
	return ((status == 0)||(status == 1));
}
#undef SCELL
#undef IDS
}
#endif 
