#ifndef slic3r_Polyline_hpp_
#define slic3r_Polyline_hpp_

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
}

#include "Point.hpp"
#include <algorithm>

namespace Slic3r {

class Polyline
{
    public:
    Points points;
    void scale(double factor);
    void translate(double x, double y);
    void rotate(double angle, Point* center);
    void reverse();
};

typedef std::vector<Polyline> Polylines;

void
Polyline::scale(double factor)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).scale(factor);
    }
}

void
Polyline::translate(double x, double y)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).translate(x, y);
    }
}

void
Polyline::rotate(double angle, Point* center)
{
    for (Points::iterator it = points.begin(); it != points.end(); ++it) {
        (*it).rotate(angle, center);
    }
}

void
Polyline::reverse()
{
    std::reverse(this->points.begin(), this->points.end());
}

void
perl2polyline(SV* poly_sv, Polyline& poly)
{
    AV* poly_av = (AV*)SvRV(poly_sv);
    const unsigned int num_points = av_len(poly_av)+1;
    poly.points.resize(num_points);
    
    for (unsigned int i = 0; i < num_points; i++) {
        SV** point_sv = av_fetch(poly_av, i, 0);
        perl2point(*point_sv, poly.points[i]);
    }
}

void
perl2polyline_check(SV* poly_sv, Polyline& poly)
{
    if (sv_isobject(poly_sv) && (SvTYPE(SvRV(poly_sv)) == SVt_PVMG)) {
        poly = *(Polyline*)SvIV((SV*)SvRV( poly_sv ));
    } else {
        perl2polyline(poly_sv, poly);
    }
}

SV*
polyline2perl(Polyline& poly) {
    const unsigned int num_points = poly.points.size();
    AV* av = newAV();
    av_extend(av, num_points-1);
    for (unsigned int i = 0; i < num_points; i++) {
        av_store(av, i, point2perl(poly.points[i]));
    }
    return sv_bless(newRV_noinc((SV*)av), gv_stashpv("Slic3r::Polyline", GV_ADD));
}

}

#endif
