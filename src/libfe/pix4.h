// -*-C++-*-
// ==========================================================================
// Module libfe
// File pix4.h
// Declaration of class Pixel4
//
// 4-noded regular pixel element aligned with global coordinate axes,
// to for a regular mesh grid.
//
// Node arrangement:
//
//    y ^
//      |
//    N2               N3              Side       contains nodes
//      +-------------+                0 (y=0)    0,1
//      |             |                1 (y=1)    2,3
//      |             |                2 (x=0)    0,2
//      |             |                3 (x=1)    1,3
//      |             |                -------------------------
//      |             |                Node coords of local element:
//      |             |                N0 = (0,0)
//      |             |                N1 = (1,0)
//      +-------------+ --> x          N2 = (0,1)
//    N0               N1              N3 = (1,1)
//
// Inheritance:
// ------------
// Element
// ---> Element_Structured
//      ---> Element_Structured_2D
//           ---> Pixel4
// ==========================================================================

#ifndef __PIX4_H
#define __PIX4_H

class FELIB Pixel4: public Element_Structured_2D {
public:

    Pixel4 () { Node = new int[nNode()]; }
    Pixel4 (const Pixel4 &el);
    ~Pixel4 () { delete[]Node; }

    void Initialise (const NodeList &nlist);

    inline BYTE Type() const { return ELID_PIX4; }
    inline unsigned long GetCaps() const { return 0; }
    inline int nNode() const { return 4; }
    inline int nSide() const { return 4; }
    inline int nSideNode (int /*side*/) const { return 2; }
    int SideNode (int side, int node) const;
  
    inline double Size() const { return size; }

    Point Local (const NodeList &nlist, const Point &glob) const
    { return Local (glob); }
    Point Local (const Point &glob) const;
    Point NodeLocal (int node) const;
    const RVector &LNormal (int side) const;
    inline RVector DirectionCosine (int side, RDenseMatrix &jacin)
    { return LNormal (side); }
    bool LContains (const Point &loc, bool pad = true) const;
    bool GContains (const Point &glob, const NodeList&) const;

    RVector LocalShapeF (const Point &loc) const;
    RDenseMatrix LocalShapeD (const Point &loc) const;
    RVector GlobalShapeF (const Point &glob) const
    { return LocalShapeF (Local (glob)); }
    RDenseMatrix GlobalShapeD (const Point &glob) const
    { return LocalShapeD (Local (glob)); }
    RVector GlobalShapeF (const NodeList &nlist, const Point &glob) const
    { return GlobalShapeF (glob); }
    RDenseMatrix GlobalShapeD (const NodeList &nlist, const Point &glob) const
    { return GlobalShapeD (glob); }

    inline double IntF (int i) const
    { return 0.25*dx*dy; }
    inline double IntFF (int i, int j) const
    { return dx*dy/9.0; }
    inline RSymMatrix IntFF() const
    { RSymMatrix ff(4);
      ff = dx*dy/9.0;
      return ff;
    }
    inline double IntFFF (int i, int j, int k) const
    { return dx*dy*0.0625; }
    inline double IntPFF (int i, int j, const RVector &P) const
    { double sump = 0.0;
      for (int k = 0; k < 4; k++) sump += P[Node[k]];
      return sump * dx*dy*0.0625;
    }
    inline RSymMatrix IntPFF (const RVector& P) const
    { double sump = 0.0;
      for (int k = 0; k < 4; k++) sump += P[Node[k]];
      RSymMatrix pff(4);
      pff = sump *dx*dy*0.0625;
      return pff;
    }
    inline RSymMatrix IntDD () const { return intdd; }
    inline double IntDD (int i, int j) const { return intdd(i,j); }
    inline double IntFDD (int i, int j, int k) const { return intfdd[i](j,k); }
    inline double IntPDD (int i, int j, const RVector &P) const
    { double res = 0.0;
      for (int k = 0; k < 4; k++) res += P[Node[k]] * intfdd[k](i,j);
      return res;
    }
    inline RSymMatrix IntPDD (const RVector& P) const
    { RSymMatrix pdd(4);
      for (int i = 0; i < 4; i++)
	for (int j = 0; j < 4; j++)
	  pdd(i,j) = IntPDD(i,j,P);
      return pdd;
    }

    // mixed derivatives

    RSymMatrix Intdd() const;
    // Int du_j/dx_l du_k/dx_m dr
    double IntFd (int i, int j, int k) const;
    // Int [u_i du_j/dx_k] dr
    double IntFdd (int i, int j, int k, int l, int m) const;
    // Int u_i du_j/dx_l du_k/dx_m dr
    double IntPdd (const RVector &p, int j, int k, int l, int m) const;
    // Int f(r) du_j/dx_l du_k/dx_m dr
    // where f(r) is given as a nodal vector
    double IntFfd (int i, int j, int k, int l) const;
    // Int u_i u_j du_k/dx_l dr
    double IntPfd(const RVector &p,int j,int k,int l) const;
    // Int f(r) u_j du_k/du_l dr

    double BndIntFFSide (int i, int j, int sd);
    double BndIntFF (int i, int j);
    RSymMatrix BndIntFF () const;
    RSymMatrix BndIntPFF (const RVector &P) const
    { xERROR(Not implemented); return RSymMatrix(); }
    double BndIntPFF (int i, int j, const RVector &P) const;

    int GlobalIntersection (const NodeList &nlist, const Point &p1,
        const Point &p2, Point **list)
    { xERROR(Not implemented); return 0; }
    int Intersection (const Point &p1, const Point &p2, Point** pi)
    { xERROR(Not implemented); return 0; }

protected:
    void ComputeIntDD () const;
    void ComputeIntFDD () const;
    void ComputeBndIntFF () const;
    void ComputeBndIntFFF () const;

private:
    double x0, y0; // global coords of node 0

    // shared properties
    static double dx;
    static double dy; // pixel edge lengths
    static double size;   // pixel area
    static RSymMatrix intdd;
    static RSymMatrix intfdd[4];
    static RSymMatrix *bndintff;
    static RDenseMatrix bndintfff[4][4];
};

#endif // !__PIX4_H