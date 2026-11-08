
// ================================================================================
// ##################################################
// # Bot: t.me/imguisource_bot
// # Main: t.me/ImGui_Source_FIXNEY (ImGui Source)
// # Two: t.me/il2cpp_src (shit src 🧨)
// ##################################################
// ================================================================================

#include "screen_geometry.hpp"
#include <cmath>
#include <limits>
namespace screen_geometry {
namespace {
struct Math {
    static constexpr float Zero=0,One=1,Half=.5f,MinWidth=1,MinHeight=12;
    static constexpr float MaximumWidth=1.5f,MaximumHeight=1.75f,MinimumLimit=120;
    static constexpr float PlayerHeight=1.7f,WidthRatio=.25f;
    static constexpr float TopFraction=.16f,TopExtra=6,TopMin=10,TopMax=500;
    static constexpr float BottomFraction=.08f,BottomExtra=3,BottomMin=5,BottomMax=300;
    static constexpr float SideFraction=.12f,SideExtra=4,SideMin=6,SideMax=300,BoundsRatio=.6f;
    static constexpr std::size_t Rows=4,Axes=3,Stride=4,IgnoredBegin=4,IgnoredEnd=10;
};
float safe_scale(float value) noexcept {return std::isfinite(value)&&value>Math::Zero?value:Math::One;}
float ratio(Vec2 value) noexcept {return safe_scale(value.x)/safe_scale(value.y);}
float clamp(float value,float lower,float upper) noexcept {
    if(value>upper)value=upper;
    if(value<lower)value=lower;
    return value;
}
bool finite(Vec2 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y);}
}
Projection prepare(const Matrix& m,const Matrix& t,Vec2 display) noexcept {
    Projection out{};out.halfDisplay={display.x*Math::Half,display.y*Math::Half};
    for(std::size_t row=0;row<Math::Rows;++row)for(std::size_t column=0;column<Math::Rows;++column){
        const auto base=column*Math::Stride;
        float value=std::fma(m[row],t[base],m[row+4]*t[base+1]);
        value=std::fma(m[row+8],t[base+2],value);
        if(column==Math::Axes)value+=m[row+12];
        out.combined[row*Math::Stride+column]=value;
    }
    for(std::size_t axis=0;axis<Math::Axes;++axis){
        const auto base=axis*Math::Stride;
        const float square=std::fma(t[base+2],t[base+2],std::fma(t[base],t[base],t[base+1]*t[base+1]));
        const float length=std::sqrt(square);
        if(std::isfinite(length)&&length>out.maximumScale)out.maximumScale=length;
    }
    return out;
}
bool project(Vec3 p,const Matrix& m,Vec2 display,Vec2& out) noexcept {
    if(!std::isfinite(p.x)||!std::isfinite(p.y)||!std::isfinite(p.z))return false;
    const float x=m[12]+std::fma(m[8],p.z,std::fma(m[0],p.x,m[4]*p.y));
    const float y=m[13]+std::fma(m[9],p.z,std::fma(m[1],p.x,m[5]*p.y));
    if(!finite({x,y}))return false;
    const float w=m[15]+std::fma(m[11],p.z,std::fma(m[3],p.x,m[7]*p.y));
    if(!std::isfinite(w)||w<Config::NearW)return false;
    const float inverse=Math::One/w;
    out={std::fma(x,inverse,Math::One)*Math::Half*display.x,std::fma(-y,inverse,Math::One)*Math::Half*display.y};
    return finite(out);
}
bool validate(const Rect& r,Vec2 display,Vec2 scale) noexcept {
    if(!finite(r.minimum)||!finite(r.maximum))return false;
    const float width=r.maximum.x-r.minimum.x,height=r.maximum.y-r.minimum.y;
    if(width<=Math::MinWidth||height<Math::MinHeight)return false;
    float maxWidth=display.x*Math::MaximumWidth*safe_scale(scale.x);
    float maxHeight=display.y*Math::MaximumHeight*safe_scale(scale.y);
    if(maxWidth<=Math::MinimumLimit)maxWidth=Math::MinimumLimit;
    if(maxHeight<=Math::MinimumLimit)maxHeight=Math::MinimumLimit;
    return width<=maxWidth&&height<=maxHeight&&Math::Zero<=r.maximum.x&&r.minimum.x<=display.x&&Math::Zero<=r.maximum.y&&r.minimum.y<=display.y;
}
bool simple_bounds(float stroke,Vec3 p,const Matrix& matrix,Vec2 display,Vec2 scale,Rect& out) noexcept {
    Vec2 a{},b{};
    const bool top=project({p.x,p.y+Math::PlayerHeight,p.z},matrix,display,a);
    const bool bottom=project(p,matrix,display,b);
    if(!top||!bottom)return false;
    const float height=std::fabs(a.y-b.y);
    if(height<Math::MinHeight||!std::isfinite(height))return false;
    const float minimum=a.x<=b.x?a.x:b.x,maximum=a.x<=b.x?b.x:a.x;
    const float width=height*Math::WidthRatio*ratio(scale),padding=std::fmax(stroke,Math::One)*Math::Half;
    out.minimum={((minimum-width)-Math::One)-padding,(a.y-Math::One)-padding};
    out.maximum={padding+((maximum+width)+Math::One),padding+(b.y+Math::One)};
    return validate(out,display,scale);
}
bool skeleton_bounds(float padding,const Bones& bones,const Matrix& matrix,Vec2 display,Vec2 scale,Rect& out) noexcept {
    float minX=std::numeric_limits<float>::max(),minY=minX,lowest=minX;
    float maxX=-minX,maxY=-minX,highest=-minX,sumX=Math::Zero,sumZ=Math::Zero;
    std::size_t count=0;
    for(std::size_t index=0;index<bones.size();++index){
        if(index>=Math::IgnoredBegin&&index<Math::IgnoredEnd)continue;
        const auto& p=bones[index];
        if(p.x==Math::Zero&&p.y==Math::Zero&&p.z==Math::Zero)continue;
        Vec2 screen{};if(!project(p,matrix,display,screen))continue;
        sumX+=p.x;sumZ+=p.z;++count;
        if(screen.x<minX)minX=screen.x;
        if(screen.y<minY)minY=screen.y;
        if(screen.x>maxX)maxX=screen.x;
        if(screen.y>maxY)maxY=screen.y;
        if(p.y<lowest)lowest=p.y;
        if(p.y>highest)highest=p.y;
    }
    if(count<Config::MinimumBones)return false;
    const float inverse=Math::One/static_cast<float>(count),averageX=sumX*inverse,averageZ=sumZ*inverse;
    Vec2 bottom{},top{};
    const bool a=project({averageX,lowest,averageZ},matrix,display,bottom);
    const bool b=project({averageX,highest,averageZ},matrix,display,top);
    const float center=(a&&b?bottom.x+top.x:maxX+minX)*Math::Half,height=maxY-minY;
    const float topMargin=clamp(std::fma(height,Math::TopFraction,Math::TopExtra),Math::TopMin,Math::TopMax);
    const float bottomMargin=clamp(std::fma(height,Math::BottomFraction,Math::BottomExtra),Math::BottomMin,Math::BottomMax);
    out.minimum.y=(minY-topMargin)-padding;out.maximum.y=(maxY+bottomMargin)+padding;
    const float aspect=ratio(scale);
    const float side=clamp(std::fma(height*Math::SideFraction,aspect,Math::SideExtra),Math::SideMin,aspect*Math::SideMax);
    out.minimum.x=(minX-side)-padding;out.maximum.x=maxX+side+padding;
    const float minimumWidth=(out.maximum.y-out.minimum.y)*Math::BoundsRatio*aspect;
    if(minimumWidth>out.maximum.x-out.minimum.x){out.minimum.x=std::fma(-minimumWidth,Math::Half,center);out.maximum.x=std::fma(minimumWidth,Math::Half,center);}
    return validate(out,display,scale);
}
}
