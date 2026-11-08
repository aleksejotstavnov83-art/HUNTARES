
// ================================================================================
// TG: @imguisource_bot | @ImGui_Source_FIXNEY | @il2cpp_src
// ================================================================================

#pragma once
#include <algorithm>
#include <cmath>
#include <cstdio>
// ximira — skeet/fatality-style overlay for Standoff 2 (ARM64 / GLES3)

namespace ximira_theme {
    constexpr ImVec4 kWindowBg      {0.043f, 0.043f, 0.055f, 1.00f};
    constexpr ImVec4 kPanelBg       {0.055f, 0.055f, 0.071f, 1.00f};
    constexpr ImVec4 kCardBg        {0.071f, 0.071f, 0.090f, 1.00f};
    constexpr ImVec4 kBorder        {0.129f, 0.129f, 0.161f, 1.00f};
    constexpr ImVec4 kBorderDim     {0.094f, 0.094f, 0.118f, 1.00f};
    constexpr ImVec4 kText          {0.898f, 0.914f, 0.949f, 1.00f};
    constexpr ImVec4 kTextDim       {0.478f, 0.494f, 0.549f, 1.00f};
    constexpr ImVec4 kTextMuted     {0.310f, 0.325f, 0.376f, 1.00f};
    constexpr ImVec4 kAccentDefault {0.310f, 0.765f, 0.969f, 1.00f};
    constexpr ImVec4 kOnline        {0.290f, 0.870f, 0.502f, 1.00f};
}

struct EspAppearance {
    bool box=true,health=true,armor=true,weapon=true,ammo=true,fill=false,rounded=true;
    int boxStyle=0;
    float lineWidth=1.5f,fillIntensity=0.08f,cornerRadius=6.0f;
    ImVec4 boxColor  {0.310f,0.765f,0.969f,1.0f};
    ImVec4 healthColor{0.361f,0.871f,0.651f,1.0f};
    ImVec4 armorColor {0.310f,0.765f,0.969f,1.0f};
    ImVec4 textColor {0.898f,0.914f,0.949f,1.0f};
    ImVec4 accent    {0.310f,0.765f,0.969f,1.0f};
};
EspAppearance g_appearance;

float g_uiScale=1.0f;
float g_menuAnimation=0.0f;
float g_pageAnimation=1.0f;
int   g_section=1;
int   g_subsection=0;
bool  g_espEditorOpen=false;
bool  g_settingsWindowOpen=false;
int   g_settingsSection=101;
ImVec2 g_settingsWindowPos(-1.0f,-1.0f);
bool  g_settingsDrag=false;
ImVec2 g_settingsDragOffset(0.0f,0.0f);
bool  g_watermarkEnabled=true;
int   g_menuScale=3;

struct WmState { float x=-1.0f, y=-1.0f; bool drag=false; float dragOffX=0, dragOffY=0; };
WmState g_wm{};

struct SplashState { bool armed=false; bool done=false; double startTime=0.0; };
SplashState g_splash{};

#include "features/visuals/world_visuals.hpp"
#include "features/visuals/post_fx.hpp"

// ─── helpers ────────────────────────────────────────────────────────────────
float uiEase(float value,float target,float speed=18.0f){
    return value+(target-value)*(1.0f-std::exp(-speed*std::min(ImGui::GetIO().DeltaTime,0.05f)));
}
ImU32 uiColor(ImVec4 c,float alpha=1.0f){
    c.w*=alpha*ImGui::GetStyle().Alpha;
    return ImGui::ColorConvertFloat4ToU32(c);
}
ImVec2 uiAdd(ImVec2 a,float x,float y){return ImVec2(a.x+x,a.y+y);}

void uiText(ImDrawList* d,ImVec2 center,const char* text,ImU32 color,float size=0,ImFont* font=nullptr){
    if(!font)font=ImGui::GetFont();
    if(size<=0)size=21*g_uiScale;
    const ImVec2 t=font->CalcTextSizeA(size,FLT_MAX,0,text);
    const ImVec2 p(center.x-t.x*0.5f,center.y-t.y*0.5f);
    d->PushClipRect(ImGui::GetWindowPos(),ImVec2(ImGui::GetWindowPos().x+ImGui::GetWindowSize().x,ImGui::GetWindowPos().y+ImGui::GetWindowSize().y),true);
    d->AddText(font,size,p,color,text);
    d->PopClipRect();
}
void uiTextLeft(ImDrawList* d,ImVec2 p,const char* text,ImU32 color,float size=0,ImFont* font=nullptr){
    if(!font)font=ImGui::GetFont();
    if(size<=0)size=21*g_uiScale;
    d->PushClipRect(ImGui::GetWindowPos(),ImVec2(ImGui::GetWindowPos().x+ImGui::GetWindowSize().x,ImGui::GetWindowPos().y+ImGui::GetWindowSize().y),true);
    d->AddText(font,size,p,color,text);
    d->PopClipRect();
}
float uiTextWidth(const char* text,float size=0,ImFont* font=nullptr){
    if(!font)font=ImGui::GetFont();
    if(size<=0)size=21*g_uiScale;
    return font->CalcTextSizeA(size,FLT_MAX,0,text).x;
}
void uiIcon(ImDrawList* d,int icon,ImVec2 p,float size,ImU32 color){
    const auto& data=lemming::icons::kIcons[std::clamp(icon,0,8)];
    for(int j=0;j<data.count;++j){
        const auto& line=lemming::icons::kSegments[data.first+j];
        const ImVec2 a(p.x+line.x1*size/24,p.y+line.y1*size/24);
        const ImVec2 b(p.x+line.x2*size/24,p.y+line.y2*size/24);
        d->AddLine(a,b,color,1.65f*size/24);
        d->AddCircleFilled(a,0.825f*size/24,color,6);
    }
}
void uiPanel(ImDrawList* d,ImVec2 a,ImVec2 b,float rounding=10){
    d->AddRectFilled(a,b,uiColor(ximira_theme::kWindowBg),rounding*g_uiScale);
    d->AddRect(a,b,uiColor(ximira_theme::kBorder),rounding*g_uiScale,0,1.0f);
}
void uiCard(ImDrawList* d,ImVec2 a,ImVec2 b){
    d->AddRectFilled(a,b,uiColor(ximira_theme::kCardBg),6*g_uiScale);
    d->AddRect(a,b,uiColor(ximira_theme::kBorderDim),6*g_uiScale,0,1.0f);
}
void uiCardWithHeader(ImDrawList* d,ImVec2 a,ImVec2 b,int icon,const char* title,const char* subtitle){
    d->AddRectFilled(a,b,uiColor(ximira_theme::kCardBg),6*g_uiScale);
    d->AddRect(a,b,uiColor(ximira_theme::kBorderDim),6*g_uiScale,0,1.0f);
    const float headerH=34*g_uiScale;
    if(icon>=0){
        uiIcon(d,icon,uiAdd(a,12*g_uiScale,(headerH-18*g_uiScale)*0.5f),18*g_uiScale,uiColor(g_appearance.accent));
    }
    const float tx=icon>=0?36*g_uiScale:14*g_uiScale;
    uiTextLeft(d,uiAdd(a,tx,(headerH-17*g_uiScale)*0.5f),title,uiColor(ximira_theme::kText),16*g_uiScale,g_uiSmallFont);
    if(subtitle&&subtitle[0]){
        const float tw=uiTextWidth(title,16*g_uiScale,g_uiSmallFont);
        uiTextLeft(d,uiAdd(a,tx+tw+10*g_uiScale,(headerH-15*g_uiScale)*0.5f),subtitle,uiColor(ximira_theme::kTextMuted),13*g_uiScale,g_uiSmallFont);
    }
    d->AddLine(uiAdd(a,10*g_uiScale,headerH),ImVec2(b.x-10*g_uiScale,a.y+headerH),uiColor(ximira_theme::kBorderDim),1.0f);
}

// ─── toggle ────────────────────────────────────────────────────────────────
bool uiToggle(const char* label,bool& value){
    ImGui::PushID(label);
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const float w=ImGui::GetContentRegionAvail().x;
    const float h=28*g_uiScale;
    const bool hit=ImGui::InvisibleButton("##x_check",ImVec2(w,h));
    if(hit)value=!value;

    ImGuiStorage* store=ImGui::GetStateStorage();
    auto id=ImGui::GetItemID();
    float t=uiEase(store->GetFloat(id,value?1:0),value?1:0,22);
    store->SetFloat(id,t);

    auto* d=ImGui::GetWindowDrawList();
    const bool hovered=ImGui::IsItemHovered();
    if(hovered) d->AddRectFilled(uiAdd(a,-6*g_uiScale,-2*g_uiScale),uiAdd(a,w,h+2*g_uiScale),
        uiColor(g_appearance.accent,0.045f),4*g_uiScale);

    const float trackW=32*g_uiScale,trackH=17*g_uiScale;
    const ImVec2 tp=uiAdd(a,w-trackW,(h-trackH)*0.5f);
    const ImVec4 trackOff(0.157f,0.157f,0.180f,1.0f);
    ImVec4 track(trackOff.x+(g_appearance.accent.x-trackOff.x)*t,
                 trackOff.y+(g_appearance.accent.y-trackOff.y)*t,
                 trackOff.z+(g_appearance.accent.z-trackOff.z)*t,1.0f);
    d->AddRectFilled(tp,uiAdd(tp,trackW,trackH),uiColor(track),trackH*0.5f);

    const float knobR=trackH*0.5f-1.5f*g_uiScale;
    const ImVec2 knob=uiAdd(tp,knobR+2*g_uiScale+(trackW-trackH)*t,trackH*0.5f);
    d->AddCircleFilled(knob,knobR,uiColor(ImVec4(0.98f,0.98f,1.0f,1.0f)),20);

    uiTextLeft(d,uiAdd(a,0,(h-17*g_uiScale)*0.5f),label,
        uiColor(value?ximira_theme::kText:ximira_theme::kTextDim),16*g_uiScale,g_uiSmallFont);

    ImGui::PopID();return hit;
}

// ─── slider ────────────────────────────────────────────────────────────────
void uiSlider(const char* id,const char* label,float& value,float lo,float hi,const char* format){
    ImGui::PushID(id);
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const float w=std::max(1.0f,ImGui::GetContentRegionAvail().x);
    const float h=58.0f*g_uiScale;
    const float trackY=a.y+39.0f*g_uiScale;
    const float trackH=5.0f*g_uiScale;
    const float trackX=a.x+9.0f*g_uiScale;
    const float trackW=std::max(1.0f,w-18.0f*g_uiScale);
    ImGui::InvisibleButton("##slider_hitbox",ImVec2(w,h));
    const bool hovered=ImGui::IsItemHovered();
    const bool active=ImGui::IsItemActive();
    const ImGuiID sliderId=ImGui::GetItemID();
    if(active && ImGui::GetIO().MouseDown[0] && hi>lo){
        const float mouseX=ImGui::GetIO().MousePos.x;
        const float t=std::clamp((mouseX-trackX)/trackW,0.0f,1.0f);
        value=lo+(hi-lo)*t;
    }
    value=std::clamp(value,lo,hi);
    auto* d=ImGui::GetWindowDrawList();
    if(hovered) d->AddRectFilled(uiAdd(a,0,1*g_uiScale),uiAdd(a,w,h-2*g_uiScale),uiColor(g_appearance.accent,0.035f),5*g_uiScale);
    uiTextLeft(d,ImVec2(a.x,a.y+1*g_uiScale),label,uiColor(ximira_theme::kText,0.94f),15*g_uiScale,g_uiSmallFont);
    char buf[64];std::snprintf(buf,sizeof(buf),format,value);
    const float tw=uiTextWidth(buf,15*g_uiScale,g_uiSmallFont);
    uiTextLeft(d,ImVec2(a.x+w-tw,a.y+1*g_uiScale),buf,uiColor(g_appearance.accent,0.98f),15*g_uiScale,g_uiSmallFont);
    const ImVec2 t0(trackX,trackY),t1(trackX+trackW,trackY+trackH);
    d->AddRectFilled(t0,t1,uiColor(ImVec4(0.14f,0.14f,0.17f,1.0f)),trackH*0.5f);
    const float frac=(hi>lo)?std::clamp((value-lo)/(hi-lo),0.0f,1.0f):0.0f;
    // Keep animation state tied to this control's unique ImGui ID, but never animate the value itself.
    ImGuiStorage* store=ImGui::GetStateStorage();
    const float visualFrac=uiEase(store->GetFloat(sliderId,frac),frac,active?60.0f:30.0f);
    store->SetFloat(sliderId,visualFrac);
    const ImVec2 gp(trackX+trackW*visualFrac,trackY+trackH*0.5f);
    d->AddRectFilled(t0,ImVec2(gp.x,t1.y),uiColor(g_appearance.accent,0.98f),trackH*0.5f);
    d->AddCircleFilled(gp,9.0f*g_uiScale,uiColor(g_appearance.accent),24);
    d->AddCircleFilled(gp,3.7f*g_uiScale,uiColor(ximira_theme::kWindowBg),20);
    ImGui::SetCursorScreenPos(ImVec2(a.x,a.y+h+2*g_uiScale));
    ImGui::PopID();
}

void uiSliderInt(const char* id,const char* label,int& value,int lo,int hi,const char* format){
    ImGui::PushID(id);
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const float w=std::max(1.0f,ImGui::GetContentRegionAvail().x);
    const float h=44.0f*g_uiScale;
    const float trackY=a.y+28.0f*g_uiScale;
    const float trackH=4.0f*g_uiScale;
    const float trackX=a.x+8.0f*g_uiScale;
    const float trackW=std::max(1.0f,w-16.0f*g_uiScale);
    ImGui::InvisibleButton("##sliderint_hitbox",ImVec2(w,h));
    const bool hovered=ImGui::IsItemHovered();
    const bool active=ImGui::IsItemActive();
    const ImGuiID sliderId=ImGui::GetItemID();
    if(active && ImGui::GetIO().MouseDown[0] && hi>lo){
        const float t=std::clamp((ImGui::GetIO().MousePos.x-trackX)/trackW,0.0f,1.0f);
        value=lo+int(std::lround(float(hi-lo)*t));
    }
    value=std::clamp(value,lo,hi);
    auto* d=ImGui::GetWindowDrawList();
    if(hovered) d->AddRectFilled(uiAdd(a,0,0),uiAdd(a,w,h),uiColor(g_appearance.accent,0.035f),5*g_uiScale);
    uiTextLeft(d,ImVec2(a.x,a.y),label,uiColor(ximira_theme::kText,0.94f),15*g_uiScale,g_uiSmallFont);
    char buf[64];std::snprintf(buf,sizeof(buf),format,value);
    const float tw=uiTextWidth(buf,15*g_uiScale,g_uiSmallFont);
    uiTextLeft(d,ImVec2(a.x+w-tw,a.y),buf,uiColor(g_appearance.accent,0.98f),15*g_uiScale,g_uiSmallFont);
    const ImVec2 t0(trackX,trackY),t1(trackX+trackW,trackY+trackH);
    d->AddRectFilled(t0,t1,uiColor(ImVec4(0.14f,0.14f,0.17f,1.0f)),trackH*0.5f);
    const float frac=(hi>lo)?std::clamp(float(value-lo)/float(hi-lo),0.0f,1.0f):0.0f;
    ImGuiStorage* store=ImGui::GetStateStorage();
    const float visualFrac=uiEase(store->GetFloat(sliderId,frac),frac,active?60.0f:30.0f);
    store->SetFloat(sliderId,visualFrac);
    const ImVec2 gp(trackX+trackW*visualFrac,trackY+trackH*0.5f);
    d->AddRectFilled(t0,ImVec2(gp.x,t1.y),uiColor(g_appearance.accent,0.98f),trackH*0.5f);
    d->AddCircleFilled(gp,7.5f*g_uiScale,uiColor(g_appearance.accent),22);
    d->AddCircleFilled(gp,3.0f*g_uiScale,uiColor(ximira_theme::kWindowBg),18);
    ImGui::SetCursorScreenPos(ImVec2(a.x,a.y+h+2*g_uiScale));
    ImGui::PopID();
}

bool uiCombo(const char* id,const char* label,int& value,const char* const items[],int itemsCount){
    ImGui::PushID(id);
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const float w=ImGui::GetContentRegionAvail().x;
    const float h=26*g_uiScale;
    const float labelW=w*0.5f;
    uiTextLeft(ImGui::GetWindowDrawList(),uiAdd(a,0,(h-15*g_uiScale)*0.5f),label,
        uiColor(ximira_theme::kText,0.92f),15*g_uiScale,g_uiSmallFont);
    ImGui::SetCursorScreenPos(uiAdd(a,labelW,0));
    ImGui::SetNextItemWidth(w-labelW);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,ImVec2(8*g_uiScale,4*g_uiScale));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,uiColor(ImVec4(0.071f,0.071f,0.086f,1.0f)));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,uiColor(ImVec4(0.086f,0.086f,0.110f,1.0f)));
    ImGui::PushStyleColor(ImGuiCol_FrameBgActive,uiColor(ImVec4(0.086f,0.086f,0.110f,1.0f)));
    ImGui::PushStyleColor(ImGuiCol_PopupBg,uiColor(ImVec4(0.071f,0.071f,0.086f,1.0f)));
    ImGui::PushStyleColor(ImGuiCol_Border,uiColor(ximira_theme::kBorder));
    ImGui::PushStyleColor(ImGuiCol_Header,uiColor(g_appearance.accent,0.35f));
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered,uiColor(g_appearance.accent,0.55f));
    ImGui::PushStyleColor(ImGuiCol_HeaderActive,uiColor(g_appearance.accent,0.65f));
    ImGui::PushStyleColor(ImGuiCol_Text,uiColor(ximira_theme::kText));
    bool changed=ImGui::Combo("##x_combo",&value,items,itemsCount);
    ImGui::PopStyleColor(9);
    ImGui::PopStyleVar();
    ImGui::SetCursorScreenPos(uiAdd(a,0,h+4*g_uiScale));
    ImGui::PopID();
    return changed;
}

void uiColorRow(const char* label,ImVec4& c){
    ImGui::PushID(label);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    float w=ImGui::GetContentRegionAvail().x;
    const float h=26*g_uiScale;
    uiTextLeft(ImGui::GetWindowDrawList(),uiAdd(p,0,(h-15*g_uiScale)*0.5f),label,
        uiColor(ximira_theme::kText,0.92f),15*g_uiScale,g_uiSmallFont);
    ImGui::SetCursorScreenPos(uiAdd(p,w-26*g_uiScale,0));
    ImGui::PushStyleColor(ImGuiCol_Border,uiColor(ximira_theme::kBorder));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,4.0f*g_uiScale);
    if(ImGui::ColorButton("##x_color",c,ImGuiColorEditFlags_NoTooltip,ImVec2(26*g_uiScale,26*g_uiScale)))
        ImGui::OpenPopup("##x_picker");
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    if(ImGui::BeginPopup("##x_picker")){
        ImGui::SetNextItemWidth(240*g_uiScale);
        ImGui::ColorPicker4("##rgba",&c.x,
            ImGuiColorEditFlags_AlphaBar|ImGuiColorEditFlags_DisplayHex|ImGuiColorEditFlags_NoSidePreview);
        ImGui::EndPopup();
    }
    ImGui::SetCursorScreenPos(uiAdd(p,0,h+4*g_uiScale));
    ImGui::PopID();
}

void uiColorRowFloat4(const char* label,float c[4]){
    ImVec4 v{c[0],c[1],c[2],c[3]};
    ImGui::PushID(label);
    const ImVec2 p=ImGui::GetCursorScreenPos();
    float w=ImGui::GetContentRegionAvail().x;
    const float h=26*g_uiScale;
    uiTextLeft(ImGui::GetWindowDrawList(),uiAdd(p,0,(h-15*g_uiScale)*0.5f),label,
        uiColor(ximira_theme::kText,0.92f),15*g_uiScale,g_uiSmallFont);
    ImGui::SetCursorScreenPos(uiAdd(p,w-26*g_uiScale,0));
    ImGui::PushStyleColor(ImGuiCol_Border,uiColor(ximira_theme::kBorder));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize,1.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,4.0f*g_uiScale);
    if(ImGui::ColorButton("##x_color4",v,ImGuiColorEditFlags_NoTooltip,ImVec2(26*g_uiScale,26*g_uiScale)))
        ImGui::OpenPopup("##x_picker4");
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
    if(ImGui::BeginPopup("##x_picker4")){
        ImGui::SetNextItemWidth(240*g_uiScale);
        ImGui::ColorPicker4("##rgba4",&v.x,
            ImGuiColorEditFlags_AlphaBar|ImGuiColorEditFlags_DisplayHex|ImGuiColorEditFlags_NoSidePreview);
        ImGui::EndPopup();
    }
    c[0]=v.x; c[1]=v.y; c[2]=v.z; c[3]=v.w;
    ImGui::SetCursorScreenPos(uiAdd(p,0,h+4*g_uiScale));
    ImGui::PopID();
}

bool uiRailIcon(const char* label,int icon,bool selected,float width){
    const float h=42*g_uiScale;
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const bool pressed=ImGui::InvisibleButton(label,ImVec2(width,h));
    const bool hovered=ImGui::IsItemHovered();
    auto* d=ImGui::GetWindowDrawList();

    if(selected){
        d->AddRectFilled(uiAdd(a,6*g_uiScale,3*g_uiScale),uiAdd(a,width-6*g_uiScale,h-3*g_uiScale),
            uiColor(g_appearance.accent,0.12f),5*g_uiScale);
        d->AddRectFilled(uiAdd(a,0,8*g_uiScale),uiAdd(a,2.0f*g_uiScale,h-8*g_uiScale),
            uiColor(g_appearance.accent),1.0f*g_uiScale);
    }else if(hovered){
        d->AddRectFilled(uiAdd(a,6*g_uiScale,3*g_uiScale),uiAdd(a,width-6*g_uiScale,h-3*g_uiScale),
            uiColor(ImVec4(0.086f,0.086f,0.110f,1.0f)),5*g_uiScale);
    }

    const float is=16*g_uiScale;
    uiIcon(d,icon,uiAdd(a,14*g_uiScale,(h-is)*0.5f),is,
        uiColor(selected?g_appearance.accent:(hovered?ximira_theme::kText:ximira_theme::kTextDim)));
    uiTextLeft(d,uiAdd(a,14*g_uiScale+is+10*g_uiScale,(h-16*g_uiScale)*0.5f),label,
        uiColor(selected?ximira_theme::kText:(hovered?ximira_theme::kText:ximira_theme::kTextDim)),
        16*g_uiScale,g_uiSmallFont);

    return pressed;
}

bool uiChoiceButton(const char* id,const char* label,bool selected,ImVec2 size){
    ImGui::PushID(id);
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const bool pressed=ImGui::InvisibleButton("##x_choice",size);
    auto* d=ImGui::GetWindowDrawList();
    const bool hovered=ImGui::IsItemHovered();
    const ImVec4 bg=selected?ImVec4(0.086f,0.086f,0.110f,1.0f):ImVec4(0.071f,0.071f,0.086f,1.0f);
    d->AddRectFilled(a,uiAdd(a,size.x,size.y),uiColor(bg),5*g_uiScale);
    d->AddRect(a,uiAdd(a,size.x,size.y),
        uiColor(selected?g_appearance.accent:(hovered?g_appearance.accent:ximira_theme::kBorder),
                selected?0.9f:(hovered?0.5f:1.0f)),
        5*g_uiScale,0,1.0f);
    uiText(d,uiAdd(a,size.x*.5f,size.y*.5f),label,
        uiColor(selected?ximira_theme::kText:ximira_theme::kTextDim),15*g_uiScale,g_uiSmallFont);
    ImGui::PopID();return pressed;
}

bool uiActionButton(const char* id,const char* title,const char* subtitle,int icon){
    ImGui::PushID(id);
    const ImVec2 a=ImGui::GetCursorScreenPos();
    const float w=ImGui::GetContentRegionAvail().x;
    const float h=52*g_uiScale;
    const bool pressed=ImGui::InvisibleButton("##x_action",ImVec2(w,h));
    auto* d=ImGui::GetWindowDrawList();
    const bool hovered=ImGui::IsItemHovered();
    d->AddRectFilled(a,uiAdd(a,w,h),uiColor(ximira_theme::kCardBg),5*g_uiScale);
    d->AddRect(a,uiAdd(a,w,h),
        uiColor(hovered?g_appearance.accent:ximira_theme::kBorderDim,hovered?0.8f:1.0f),
        5*g_uiScale,0,1.0f);
    if(icon>=0) uiIcon(d,icon,uiAdd(a,16*g_uiScale,(h-20*g_uiScale)*0.5f),20*g_uiScale,
        uiColor(g_appearance.accent));
    const float tx=icon>=0?46*g_uiScale:16*g_uiScale;
    uiTextLeft(d,uiAdd(a,tx,11*g_uiScale),title,uiColor(ximira_theme::kText),17*g_uiScale,g_uiTitleFont);
    uiTextLeft(d,uiAdd(a,tx,31*g_uiScale),subtitle,uiColor(ximira_theme::kTextMuted),13*g_uiScale,g_uiSmallFont);
    ImGui::PopID();return pressed;
}

void uiGroupBegin(ImDrawList* d,ImVec2 a,ImVec2 b,int icon,const char* title,const char* subtitle){
    uiCardWithHeader(d,a,b,icon,title,subtitle);
}
// ─── ESP box ───────────────────────────────────────────────────────────────
void uiEspText(ImDrawList* d,ImVec2 center,const char* text,ImU32 color,float size){
    uiText(d,uiAdd(center,0,1),text,IM_COL32(0,0,0,235),size,g_uiSmallFont);
    uiText(d,center,text,color,size,g_uiSmallFont);
}
void drawStyledEsp(ImDrawList* d,const EspScreenBox& b){
    const ImVec2 a(b.left,b.top),z(b.right,b.bottom);
    float h=z.y-a.y,w=z.x-a.x;
    if(h<8||w<2)return;
    const float center=(a.x+z.x)*0.5f,thick=g_appearance.lineWidth;
    const float rounding=g_appearance.rounded?g_appearance.cornerRadius*g_uiScale:0.0f;
    const auto boxColor=uiColor(g_appearance.boxColor);

    if(g_appearance.box){
        if(g_appearance.fill)
            d->AddRectFilled(a,z,uiColor(g_appearance.boxColor,std::clamp(g_appearance.fillIntensity,0.0f,0.35f)),rounding);
        if(g_appearance.boxStyle==0){
            d->AddRect(a,z,IM_COL32(0,0,0,170),rounding,0,thick+2);
            d->AddRect(a,z,boxColor,rounding,0,thick);
        }else{
            float l=std::min(w*0.27f,h*0.15f);
            const ImVec2 pts[4]={a,ImVec2(z.x,a.y),ImVec2(a.x,z.y),z};
            for(int i=0;i<4;++i){
                float dx=i%2?-l:l,dy=i>=2?-l:l;
                d->AddLine(pts[i],uiAdd(pts[i],dx,0),IM_COL32(0,0,0,190),thick+2);
                d->AddLine(pts[i],uiAdd(pts[i],0,dy),IM_COL32(0,0,0,190),thick+2);
                d->AddLine(pts[i],uiAdd(pts[i],dx,0),boxColor,thick);
                d->AddLine(pts[i],uiAdd(pts[i],0,dy),boxColor,thick);
            }
        }
    }

    char text[48];
    if(g_appearance.health&&b.statsValid){
        const float ratio=b.maxHp?std::clamp(float(b.hp)/b.maxHp,0.0f,1.0f):0;
        const ImVec2 u(a.x-9,a.y),v(a.x-5,z.y);
        d->AddRectFilled(uiAdd(u,-1,-1),uiAdd(v,1,1),IM_COL32(5,8,14,220),3);
        d->AddRectFilled(u,v,IM_COL32(35,41,53,220),2);
        ImVec4 c=g_appearance.healthColor,low(1.0f,0.34f,0.40f,1);
        c=ImVec4(low.x+(c.x-low.x)*ratio,low.y+(c.y-low.y)*ratio,low.z+(c.z-low.z)*ratio,1);
        d->AddRectFilled(ImVec2(u.x,z.y-h*ratio),v,uiColor(c),2);
        if(b.hp<b.maxHp){std::snprintf(text,sizeof(text),"%u",unsigned(b.hp));uiEspText(d,ImVec2(a.x-20,z.y-h*ratio),text,uiColor(c),14*g_uiScale);}
    }

    float labelY=z.y+13*g_uiScale;
    if(g_appearance.armor&&b.statsValid&&b.maxArmor>0){
        float ratio=std::clamp(float(b.armor)/b.maxArmor,0.0f,1.0f);
        d->AddRectFilled(ImVec2(a.x-1,z.y+5),ImVec2(z.x+1,z.y+10),IM_COL32(5,8,14,210),3);
        if(ratio>0)d->AddRectFilled(ImVec2(a.x,z.y+6),ImVec2(a.x+w*ratio,z.y+9),uiColor(g_appearance.armorColor),2);
        labelY+=9*g_uiScale;
    }
    if(b.weaponValid&&g_appearance.weapon){std::snprintf(text,sizeof(text),"WEAPON %u",unsigned(b.weaponId));uiEspText(d,ImVec2(center,labelY),text,uiColor(g_appearance.textColor),16*g_uiScale);labelY+=19*g_uiScale;}
    if(b.weaponValid&&g_appearance.ammo){std::snprintf(text,sizeof(text),"%u / %u",unsigned(b.magazineAmmo),unsigned(b.reserveAmmo));uiEspText(d,ImVec2(center,labelY),text,uiColor(g_appearance.textColor,.73f),14*g_uiScale);}
}

void renderEspBoxes(){
    if(!g_espBoxesEnabled.load(std::memory_order_relaxed))return;
    std::lock_guard<std::mutex> lock(g_espMutex);
    auto* d=ImGui::GetBackgroundDrawList();
    for(const auto& b:g_espScreenBoxes)drawStyledEsp(d,b);
}

#include "ui/pages/aim.hpp"

// ─── SPLASH ────────────────────────────────────────────────────────────────
void renderSplash(){
    if(g_splash.done)return;
    const double now=ImGui::GetTime();
    if(!g_splash.armed){g_splash.armed=true;g_splash.startTime=now;}
    const double t=now-g_splash.startTime;
    constexpr double T_IN=0.35,T_REVEAL=0.95,T_HOLD=2.05,T_FADE=2.65;
    if(t>=T_FADE){g_splash.done=true;return;}
    auto* d=ImGui::GetForegroundDrawList();
    const auto& io=ImGui::GetIO();
    const float W=io.DisplaySize.x,H=io.DisplaySize.y,cx=W*.5f,cy=H*.5f;
    float fade=1.0f;
    if(t<T_IN)fade=float(t/T_IN);
    else if(t>T_HOLD)fade=float(1.0-(t-T_HOLD)/(T_FADE-T_HOLD));
    fade=std::clamp(fade,0.0f,1.0f);
    d->AddRectFilled(ImVec2(0,0),ImVec2(W,H),IM_COL32(5,7,13,int(210*fade)));
    // Branded Ximira reveal: restrained neon bloom, geometric mark and wordmark.
    const float pulse=0.5f+0.5f*std::sin(float(t)*4.0f);
    const float mark=std::min(W,H)*0.070f;
    const ImVec2 c(cx-mark*1.55f,cy-mark*.05f);
    const ImU32 accent=uiColor(g_appearance.accent,fade);
    d->AddCircleFilled(c,mark*1.30f,uiColor(g_appearance.accent,fade*(0.06f+0.04f*pulse)),48);
    d->AddCircle(c,mark*1.10f,uiColor(g_appearance.accent,fade*.65f),48,1.2f);
    d->AddLine(ImVec2(c.x-mark*.52f,c.y-mark*.52f),ImVec2(c.x+mark*.52f,c.y+mark*.52f),accent,mark*.13f);
    d->AddLine(ImVec2(c.x+mark*.52f,c.y-mark*.52f),ImVec2(c.x-mark*.52f,c.y+mark*.52f),accent,mark*.13f);
    const float wordSize=std::clamp(std::min(W,H)*0.075f,28.0f,72.0f);
    const char* word="XIMIRA";
    ImFont* titleFont=g_uiTitleFont?g_uiTitleFont:ImGui::GetFont();
    ImFont* smallFont=g_uiSmallFont?g_uiSmallFont:ImGui::GetFont();
    const ImVec2 textSize=titleFont->CalcTextSizeA(wordSize,FLT_MAX,0,word);
    const ImVec2 textPos(cx+mark*.15f,cy-textSize.y*.62f);
    const float reveal=std::clamp(float((t-T_IN)/(T_REVEAL-T_IN)),0.0f,1.0f);
    d->AddText(titleFont,wordSize,textPos,uiColor(ximira_theme::kText,fade*reveal),word);
    const char* sub="PREMIUM  /  INITIALIZED";
    const float subSize=std::clamp(wordSize*.20f,10.0f,15.0f);
    d->AddText(smallFont,subSize,ImVec2(textPos.x,textPos.y+textSize.y+8.0f),uiColor(ximira_theme::kTextMuted,fade*reveal),sub);
    const float lineW=std::min(W*.36f,360.0f)*reveal;
    const float lineY=cy+mark*1.12f;
    d->AddRectFilled(ImVec2(cx-lineW*.5f,lineY),ImVec2(cx+lineW*.5f,lineY+2.0f),uiColor(g_appearance.accent,fade*.88f),1.0f);
    const float progress=std::clamp(float(t/T_FADE),0.0f,1.0f);
    d->AddRectFilled(ImVec2(cx-lineW*.5f,lineY+5.0f),ImVec2(cx-lineW*.5f+lineW*progress,lineY+6.5f),uiColor(g_appearance.accent,fade*.45f),1.0f);
}

// ─── WATERMARK (fixed) ─────────────────────────────────────────────────────
void renderWatermark(){
    if(!g_watermarkEnabled)return;
    auto& io=ImGui::GetIO();
    const float w=224.0f,h=42.0f;

    if(g_wm.x<0){ g_wm.x=io.DisplaySize.x-w-20.0f; g_wm.y=io.DisplaySize.y-h-20.0f; }

    ImGui::SetNextWindowPos(ImVec2(g_wm.x,g_wm.y),ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(w,h),ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
    ImGui::Begin("##ximira_wm",nullptr,
        ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|
        ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBackground|
        ImGuiWindowFlags_NoFocusOnAppearing);

    ImGui::InvisibleButton("##wm_hit",ImVec2(w,h));
    const bool hovered=ImGui::IsItemHovered();
    const bool active=ImGui::IsItemActive();
    const ImVec2 mp=io.MousePos;

    if(active&&!g_wm.drag){
        g_wm.drag=true;
        g_wm.dragOffX=mp.x-g_wm.x;
        g_wm.dragOffY=mp.y-g_wm.y;
    }
    if(g_wm.drag){
        if(io.MouseDown[0]){
            g_wm.x=mp.x-g_wm.dragOffX;
            g_wm.y=mp.y-g_wm.dragOffY;
if(g_wm.x<0)g_wm.x=0;
            if(g_wm.y<0)g_wm.y=0;
            if(g_wm.x+w>io.DisplaySize.x)g_wm.x=io.DisplaySize.x-w;
            if(g_wm.y+h>io.DisplaySize.y)g_wm.y=io.DisplaySize.y-h;
        }else{
            g_wm.drag=false;
            if(hovered){
                const bool open=!g_menuOpen.load(std::memory_order_relaxed);
                g_menuOpen.store(open,std::memory_order_relaxed);
                LOGI("ximira menu toggle: open=%d",open);
            }
        }
    }

    const ImVec2 p=ImGui::GetItemRectMin();
    auto* d=ImGui::GetForegroundDrawList();

    d->AddRectFilled(p,uiAdd(p,w,h),uiColor(ImVec4(0.035f,0.040f,0.058f,0.94f)),9.0f);
    d->AddRect(p,uiAdd(p,w,h),uiColor(g_appearance.accent,hovered?0.90f:0.50f),9.0f,0,1.0f);
    d->AddRectFilled(uiAdd(p,0,0),uiAdd(p,3.0f,h),uiColor(g_appearance.accent,0.95f),1.5f);
    const ImVec2 mark=uiAdd(p,21.0f,h*0.5f);
    const float ms=10.0f;
    d->AddLine(uiAdd(mark,-ms,-ms),uiAdd(mark,ms,ms),uiColor(g_appearance.accent),2.2f);
    d->AddLine(uiAdd(mark,ms,-ms),uiAdd(mark,-ms,ms),uiColor(g_appearance.accent),2.2f);
    ImFont* font=g_uiTitleFont?g_uiTitleFont:ImGui::GetFont();
    const float fontSize=16.0f;
    const char* text="XIMIRA";
    const ImVec2 textSize=font->CalcTextSizeA(fontSize,FLT_MAX,0,text);
    const ImVec2 textPos(p.x+39.0f,p.y+(h-textSize.y)*0.5f-1.0f);
    d->AddText(font,fontSize,textPos,uiColor(ximira_theme::kText),text);
    const float dividerX=textPos.x+textSize.x+10.0f;
    d->AddLine(ImVec2(dividerX,p.y+9.0f),ImVec2(dividerX,p.y+h-9.0f),uiColor(ximira_theme::kBorder),1.0f);
    const char* tag="PREMIUM";
    d->AddText(g_uiSmallFont?g_uiSmallFont:ImGui::GetFont(),10.0f,ImVec2(dividerX+10.0f,p.y+(h-10.0f)*0.5f),uiColor(ximira_theme::kTextMuted),tag);
    d->AddCircleFilled(ImVec2(p.x+w-13.0f,p.y+h*0.5f),2.8f,uiColor(g_appearance.accent),16);

    ImGui::End();ImGui::PopStyleVar();
}

// ─── ESP editor ────────────────────────────────────────────────────────────
void renderEspEditor(){
    if(!g_espEditorOpen)return;
    auto& io=ImGui::GetIO();
    const float s=g_uiScale;
    const float w=std::round(960*s),h=std::round(620*s);
    ImGui::SetNextWindowSize(ImVec2(w,h),ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2((io.DisplaySize.x-w)*.5f,(io.DisplaySize.y-h)*.5f),ImGuiCond_Once);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(14*s,14*s));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,6*s);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,7*s));
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.043f,.043f,.055f,.995f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg,ImVec4(.055f,.055f,.071f,1));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,ImVec4(.055f,.055f,.071f,1));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed,ImVec4(.055f,.055f,.071f,1));
    auto flags=ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|
               ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoScrollbar|
               ImGuiWindowFlags_NoScrollWithMouse;
    if(ImGui::Begin("ximira // ESP Editor##esp_editor",&g_espEditorOpen,flags)){
        const ImVec2 base=ImGui::GetCursorScreenPos();
        const ImVec2 avail=ImGui::GetContentRegionAvail();
        auto* d=ImGui::GetWindowDrawList();
        const float gap=12*s,previewW=280*s;
        const float colW=(avail.x-previewW-gap*2)*.5f;
        const float cardH=avail.y;
        const ImVec2 geomA=base,geomB=uiAdd(geomA,colW,cardH);
        const ImVec2 colorA=uiAdd(base,colW+gap,0),colorB=uiAdd(colorA,colW,cardH);
        const ImVec2 prevA=uiAdd(base,(colW+gap)*2,0),prevB=uiAdd(prevA,previewW,cardH);

        uiCard(d,geomA,geomB);uiCard(d,colorA,colorB);uiCard(d,prevA,prevB);

        uiText(d,ImVec2((geomA.x+geomB.x)*.5f,geomA.y+18*s),"GEOMETRY",
            uiColor(ximira_theme::kTextDim),12*s,g_uiSmallFont);
        uiText(d,ImVec2((colorA.x+colorB.x)*.5f,colorA.y+18*s),"ELEMENTS",
            uiColor(ximira_theme::kTextDim),12*s,g_uiSmallFont);
        uiText(d,ImVec2((prevA.x+prevB.x)*.5f,prevA.y+18*s),"PREVIEW",
            uiColor(ximira_theme::kTextDim),12*s,g_uiSmallFont);
ImGui::SetCursorScreenPos(uiAdd(geomA,12*s,40*s));
        ImGui::BeginChild("##esp_geometry",ImVec2(colW-24*s,cardH-54*s),
            ImGuiChildFlags_None,ImGuiWindowFlags_None);
        {
            const float choiceW=(ImGui::GetContentRegionAvail().x-8*s)*.5f;
            if(uiChoiceButton("full","Full",g_appearance.boxStyle==0,ImVec2(choiceW,32*s)))
                g_appearance.boxStyle=0;
            ImGui::SameLine(0,8*s);
            if(uiChoiceButton("corners","Corners",g_appearance.boxStyle==1,ImVec2(choiceW,32*s)))
                g_appearance.boxStyle=1;
            uiToggle("Box",g_appearance.box);
            uiToggle("Rounded",g_appearance.rounded);
            uiToggle("Soft fill",g_appearance.fill);
            uiSlider("thickness","Thickness",g_appearance.lineWidth,1.0f,4.0f,"%.1f px");
            uiSlider("fill_intensity","Fill intensity",g_appearance.fillIntensity,0.02f,0.30f,"%.2f");
            uiSlider("corner_radius","Corner radius",g_appearance.cornerRadius,0.0f,18.0f,"%.0f px");
        }
        ImGui::EndChild();

        ImGui::SetCursorScreenPos(uiAdd(colorA,12*s,40*s));
        ImGui::BeginChild("##esp_colors",ImVec2(colW-24*s,cardH-54*s),
            ImGuiChildFlags_None,ImGuiWindowFlags_None);
        {
            uiToggle("Health",g_appearance.health);
            uiToggle("Armor",g_appearance.armor);
            uiToggle("Weapon",g_appearance.weapon);
            uiToggle("Ammo",g_appearance.ammo);
            uiColorRow("Box color",g_appearance.boxColor);
            uiColorRow("Health color",g_appearance.healthColor);
            uiColorRow("Armor color",g_appearance.armorColor);
            uiColorRow("Text color",g_appearance.textColor);
        }
        ImGui::EndChild();

        EspScreenBox demo{};
        demo.left=prevA.x+87*s;demo.right=prevB.x-63*s;
        demo.top=prevA.y+70*s;demo.bottom=prevB.y-110*s;
        demo.statsValid=demo.weaponValid=demo.enemy=true;
        demo.hp=76;demo.maxHp=100;demo.armor=64;demo.maxArmor=100;
        demo.weaponId=12;demo.magazineAmmo=30;demo.reserveAmmo=90;
        const float cx=(demo.left+demo.right)*.5f,cy=demo.top+35*s;
        const ImU32 mannequin=uiColor(ImVec4(.24f,.29f,.38f,.50f));
        d->AddCircleFilled(ImVec2(cx,cy),13*s,mannequin,24);
        d->AddRectFilled(ImVec2(cx-25*s,cy+18*s),ImVec2(cx+25*s,cy+112*s),mannequin,12*s);
        d->AddRectFilled(ImVec2(cx-39*s,cy+30*s),ImVec2(cx-27*s,cy+105*s),mannequin,6*s);
        d->AddRectFilled(ImVec2(cx+27*s,cy+30*s),ImVec2(cx+39*s,cy+105*s),mannequin,6*s);
        d->AddRectFilled(ImVec2(cx-22*s,cy+105*s),ImVec2(cx-4*s,cy+190*s),mannequin,8*s);
        d->AddRectFilled(ImVec2(cx+4*s,cy+105*s),ImVec2(cx+22*s,cy+190*s),mannequin,8*s);
        drawStyledEsp(d,demo);
        uiText(d,ImVec2((prevA.x+prevB.x)*.5f,prevB.y-24*s),"Drag by title bar",
            uiColor(ximira_theme::kTextMuted),12*s,g_uiSmallFont);
    }
    ImGui::End();
    ImGui::PopStyleColor(4);
    ImGui::PopStyleVar(3);
}

// ─── Aimbot tab ────────────────────────────────────────────────────────────
void renderAimbotTab(float cx0,float y,float slide){
    auto c=lemming::aim::configuration();
    auto& s=c.normal;
    const float s_=g_uiScale;
    const float contentW=680*s_;
    const float cardW=(contentW-14*s_*3)*0.5f;
    const float rowH=230*s_;

    const ImVec2 aimA(cx0+14*s_,y+58*s_+slide),  aimB(aimA.x+cardW,aimA.y+rowH);
    const ImVec2 tgtA(cx0+14*s_*2+cardW,y+58*s_+slide), tgtB(tgtA.x+cardW,tgtA.y+rowH);
    const ImVec2 asA(cx0+14*s_,aimB.y+14*s_+slide), asB(asA.x+cardW*2+14*s_,asA.y+rowH*0.75f);

    auto* d=ImGui::GetWindowDrawList();
    uiGroupBegin(d,aimA,aimB,0,"Aimbot","Precision. Control. Advantage.");
    uiGroupBegin(d,tgtA,tgtB,3,"Target Selection","");
    uiGroupBegin(d,asA,asB,4,"Assists","RCS - Visibility - Prediction");

    ImGui::SetCursorScreenPos(uiAdd(aimA,10*s_,38*s_));
    ImGui::BeginChild("##aim_left",ImVec2(cardW-20*s_,rowH-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        uiToggle("Enable Aimbot",s.enabled);
        uiSlider("fov","FOV",s.fov,0.1f,180.0f,"%.1f deg");
        uiSlider("max_dist","Max distance",s.maxDistance,1.0f,500.0f,"%.0f m");
        uiSlider("smoothing","Smoothness",s.smoothing,0.0f,1.0f,"%.2f");
        static const char* kPriorities[]={"Angle","Distance","HP"};
        uiCombo("priority","Priority",s.priority,kPriorities,3);
    }
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(uiAdd(tgtA,10*s_,38*s_));
    ImGui::BeginChild("##aim_right",ImVec2(cardW-20*s_,rowH-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        uiToggle("Only while attacking",s.onAttack);
        uiToggle("Lock target",s.lockTarget);
        uiToggle("Visible check",s.visibleCheck);
        uiToggle("Multi-points",s.multipoints);
        uiSlider("point_scale","Multi-point scale",s.pointScale,0.0f,0.9f,"%.2f");
    }
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(uiAdd(asA,10*s_,38*s_));
    ImGui::BeginChild("##aim_assists",ImVec2(cardW*2+14*s_-20*s_,rowH*0.75f-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        const float halfW=(ImGui::GetContentRegionAvail().x-14*s_)*0.5f;
        ImGui::BeginChild("##as_left",ImVec2(halfW,0),ImGuiChildFlags_None,
            ImGuiWindowFlags_None);
        {
            uiToggle("RCS",s.rcs);
            if(s.rcs) uiSlider("rcs_strength","RCS strength",s.rcsStrength,0.0f,100.0f,"%.0f%%");
            uiToggle("Show FOV",s.showFov);
        }
        ImGui::EndChild();
        ImGui::SameLine(0,14*s_);
        ImGui::BeginChild("##as_right",ImVec2(halfW,0),ImGuiChildFlags_None,
            ImGuiWindowFlags_None);
        {
            uiToggle("Prediction",s.prediction);
            if(s.prediction) uiSlider("pred_ms","Prediction ms",s.predictionMs,0.0f,100.0f,"%.0f ms");
        }
        ImGui::EndChild();
    }
    ImGui::EndChild();

    const float bA_y=asB.y+14*s_+slide;
    const ImVec2 bA(cx0+14*s_,bA_y), bB(cx0+14*s_+contentW-28*s_,bA_y+82*s_);
    uiGroupBegin(d,bA,bB,5,"Body parts","Head / Chest / Limbs");
    ImGui::SetCursorScreenPos(uiAdd(bA,10*s_,38*s_));
    ImGui::BeginChild("##aim_bodies",ImVec2(contentW-48*s_,40*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);{
        const float bw=(ImGui::GetContentRegionAvail().x-7*s_*7)/8.0f;
        for(int i=0;i<8;++i){
            const bool on=(s.bodies&(1u<<i))!=0;
            if(uiChoiceButton(lemming::aim::kBodyNames[i],lemming::aim::kBodyNames[i],on,ImVec2(bw,26*s_))){
                s.bodies^=(1u<<i);
            }
            if(i<7)ImGui::SameLine(0,7*s_);
        }
    }
    ImGui::EndChild();

    c.normal=s;
    lemming::aim::publish(c);
}

// ─── Visuals tab ───────────────────────────────────────────────────────────
void renderVisualsTab(float cx0,float y,float slide){
    const float s_=g_uiScale;
    const float contentW=680*s_;
    const float cardW=(contentW-14*s_*3)*0.5f;
    const float cardH=210*s_;

    const ImVec2 espA(cx0+14*s_,y+58*s_+slide),  espB(espA.x+cardW,espA.y+cardH);
    const ImVec2 ovlA(cx0+14*s_*2+cardW,y+58*s_+slide), ovlB(ovlA.x+cardW,ovlA.y+cardH);

    auto* d=ImGui::GetWindowDrawList();
    uiGroupBegin(d,espA,espB,1,"ESP","Box · Fill · Round");
    uiGroupBegin(d,ovlA,ovlB,2,"Overlays","Health · Armor · Weapon");

    ImGui::SetCursorScreenPos(uiAdd(espA,10*s_,40*s_));
    ImGui::BeginChild("##vis_esp",ImVec2(cardW-20*s_,cardH-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        bool en=g_espBoxesEnabled.load();
        if(uiToggle("Enemy ESP",en)){g_espBoxesEnabled.store(en);LOGI("ximira esp=%d",en);}
        uiToggle("Box",g_appearance.box);
        uiToggle("Rounded",g_appearance.rounded);
        uiToggle("Soft fill",g_appearance.fill);
    }
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(uiAdd(ovlA,10*s_,40*s_));
    ImGui::BeginChild("##vis_ovl",ImVec2(cardW-20*s_,cardH-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        uiToggle("Health",g_appearance.health);
        uiToggle("Armor",g_appearance.armor);
        uiToggle("Weapon",g_appearance.weapon);
        uiToggle("Ammo",g_appearance.ammo);
    }
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(ImVec2(cx0+14*s_,espB.y+14*s_+slide));
    ImGui::BeginChild("##vis_editor",ImVec2(contentW-28*s_,68*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    if(uiActionButton("open_editor","Open ESP Editor",
        "Style, thickness, fill, rounding, colors and live preview",5))
        g_espEditorOpen=true;
    ImGui::EndChild();
}

// ─── Misc tab ──────────────────────────────────────────────────────────────
void renderMiscTab(float cx0,float y,float slide){
    const float s_=g_uiScale;
    const float contentW=680*s_;
    const ImVec2 aA(cx0+14*s_,y+58*s_+slide), aB(cx0+14*s_+contentW-28*s_,aA.y+120*s_);
    auto* d=ImGui::GetWindowDrawList();
    uiGroupBegin(d,aA,aB,2,"Misc","Information & environment");
    uiTextLeft(d,uiAdd(aA,20*s_,54*s_),"Reserved for future features.",
        uiColor(ximira_theme::kTextDim),14*s_,g_uiSmallFont);
    uiTextLeft(d,uiAdd(aA,20*s_,78*s_),"HUD, world tweaks, sounds, etc.",
        uiColor(ximira_theme::kTextMuted),13*s_,g_uiSmallFont);
}

// ─── World tab ─────────────────────────────────────────────────────────────
void renderWorldTab(float cx0,float y,float slide){
    const float s_=g_uiScale;
    const float contentW=680*s_;
    const float cardW=(contentW-14*s_*3)*0.5f;
    const float cardH=390*s_;
    auto* d=ImGui::GetWindowDrawList();
    auto& wm=xim::features::world_mod::config();
    const ImVec2 fogA(cx0+14*s_,y+58*s_+slide),fogB(fogA.x+cardW,fogA.y+cardH);
    const ImVec2 ambA(cx0+14*s_*2+cardW,y+58*s_+slide),ambB(ambA.x+cardW,ambA.y+cardH);
    uiGroupBegin(d,fogA,fogB,4,"Fog","");
    uiGroupBegin(d,ambA,ambB,4,"Ambient","");
    ImGui::SetCursorScreenPos(uiAdd(fogA,10*s_,40*s_));
    ImGui::BeginChild("##wf",ImVec2(cardW-20*s_,cardH-46*s_),ImGuiChildFlags_None,ImGuiWindowFlags_None);
    uiToggle("Enable Fog",wm.fog.enabled);
    uiSlider("fd","Density",wm.fog.density,0.0f,1.0f,"%.3f");
    uiSlider("fs","Start distance",wm.fog.start,0.0f,1000.0f,"%.1f");
    uiSlider("fe","End distance",wm.fog.end,0.1f,5000.0f,"%.1f");
    if(wm.fog.end < wm.fog.start + 0.1f) wm.fog.end = wm.fog.start + 0.1f;
    static const char* md[]={"Linear","Exp","Exp2"};
    int m=wm.fog.mode;
    if(uiCombo("fm","Mode",m,md,3)) wm.fog.mode=m;
    ImVec4 cv{wm.fog.color[0],wm.fog.color[1],wm.fog.color[2],wm.fog.color[3]};
    uiTextLeft(d,ImGui::GetCursorScreenPos(),"Color",uiColor(ximira_theme::kText,0.92f),15*s_,g_uiSmallFont);
    ImGui::SetCursorScreenPos(uiAdd(ImGui::GetCursorScreenPos(),cardW-20*s_-26*s_,0));
    ImGui::PushID("fcol");
    if(ImGui::ColorButton("##f",cv,0,ImVec2(26*s_,26*s_))) ImGui::OpenPopup("##fp");
    if(ImGui::BeginPopup("##fp")){ImGui::SetNextItemWidth(240*s_);ImGui::ColorPicker4("##rgba",&cv.x,ImGuiColorEditFlags_AlphaBar|ImGuiColorEditFlags_NoSidePreview);ImGui::EndPopup();}
    ImGui::PopID();
    wm.fog.color[0]=cv.x;wm.fog.color[1]=cv.y;wm.fog.color[2]=cv.z;wm.fog.color[3]=cv.w;
    ImGui::EndChild();
    ImGui::SetCursorScreenPos(uiAdd(ambA,10*s_,40*s_));
    ImGui::BeginChild("##wa",ImVec2(cardW-20*s_,cardH-46*s_),ImGuiChildFlags_None,ImGuiWindowFlags_None);
    uiToggle("Enable Ambient",wm.ambient.enabled);
    uiSlider("ai","Intensity",wm.ambient.intensity,0.0f,3.0f,"%.2f");
    ImVec4 av{wm.ambient.color[0],wm.ambient.color[1],wm.ambient.color[2],wm.ambient.color[3]};
    uiTextLeft(d,ImGui::GetCursorScreenPos(),"Color",uiColor(ximira_theme::kText,0.92f),15*s_,g_uiSmallFont);
    ImGui::SetCursorScreenPos(uiAdd(ImGui::GetCursorScreenPos(),cardW-20*s_-26*s_,0));
    ImGui::PushID("acol");
    if(ImGui::ColorButton("##a",av,0,ImVec2(26*s_,26*s_))) ImGui::OpenPopup("##ap");
    if(ImGui::BeginPopup("##ap")){ImGui::SetNextItemWidth(240*s_);ImGui::ColorPicker4("##rgba2",&av.x,ImGuiColorEditFlags_AlphaBar|ImGuiColorEditFlags_NoSidePreview);ImGui::EndPopup();}
    ImGui::PopID();
    wm.ambient.color[0]=av.x;wm.ambient.color[1]=av.y;wm.ambient.color[2]=av.z;wm.ambient.color[3]=av.w;
    ImGui::EndChild();
}

void renderConfigsTab(float cx0,float y,float slide){
    const float s_=g_uiScale;
    const float contentW=680*s_;
    const float cardW=(contentW-14*s_*3)*0.5f;
    const float cardH=210*s_;

    const ImVec2 menuA(cx0+14*s_,y+58*s_+slide),  menuB(menuA.x+cardW,menuA.y+cardH);
    const ImVec2 themeA(cx0+14*s_*2+cardW,y+58*s_+slide), themeB(themeA.x+cardW,themeA.y+cardH);

    auto* d=ImGui::GetWindowDrawList();
    uiGroupBegin(d,menuA,menuB,7,"Interface","Menu scale");
    uiGroupBegin(d,themeA,themeB,5,"Theme","Accent color");

    ImGui::SetCursorScreenPos(uiAdd(menuA,10*s_,40*s_));
    ImGui::BeginChild("##cfg_menu",ImVec2(cardW-20*s_,cardH-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        int prev=g_menuScale;
        uiSliderInt("menu_scale","Menu scale",g_menuScale,1,5,"%d");
        if(g_menuScale!=prev) LOGI("ximira menu scale=%d",g_menuScale);
        uiTextLeft(ImGui::GetWindowDrawList(),
            ImVec2(menuA.x+20*s_,menuA.y+140*s_),
            "Watermark is not affected by menu scale.",
            uiColor(ximira_theme::kTextMuted),12*s_,g_uiSmallFont);
    }
    ImGui::EndChild();

    ImGui::SetCursorScreenPos(uiAdd(themeA,10*s_,40*s_));
    ImGui::BeginChild("##cfg_theme",ImVec2(cardW-20*s_,cardH-46*s_),
        ImGuiChildFlags_None,ImGuiWindowFlags_None);
    {
        uiColorRow("Accent",g_appearance.accent);
        uiColorRow("Text",g_appearance.textColor);
    }
    ImGui::EndChild();
}

// ─── feature hub + detached settings window ────────────────────────────────
void renderFeatureCard(const char* id,const char* title,const char* subtitle,int icon,int section,ImVec2 a,ImVec2 size){
    ImGui::PushID(id);
    ImGui::SetCursorScreenPos(a);
    const bool pressed=ImGui::InvisibleButton("##feature_card",size);
    const bool hovered=ImGui::IsItemHovered();
    auto* d=ImGui::GetWindowDrawList();
    d->AddRectFilled(a,uiAdd(a,size.x,size.y),uiColor(hovered?ImVec4(0.085f,0.090f,0.115f,0.98f):ImVec4(0.060f,0.064f,0.082f,0.92f)),7*g_uiScale);
    d->AddRect(a,uiAdd(a,size.x,size.y),uiColor(hovered?g_appearance.accent:ximira_theme::kBorderDim,hovered?0.85f:1.0f),7*g_uiScale,0,1.0f);
    uiIcon(d,icon,uiAdd(a,14*g_uiScale,(size.y-20*g_uiScale)*0.5f),20*g_uiScale,uiColor(g_appearance.accent));
    uiTextLeft(d,uiAdd(a,44*g_uiScale,10*g_uiScale),title,uiColor(ximira_theme::kText),15*g_uiScale,g_uiTitleFont);
    uiTextLeft(d,uiAdd(a,44*g_uiScale,31*g_uiScale),subtitle,uiColor(ximira_theme::kTextMuted),11.5f*g_uiScale,g_uiSmallFont);
    if(pressed){g_settingsSection=section;g_settingsWindowOpen=true;g_settingsWindowPos=ImVec2(-1,-1);}
    ImGui::PopID();
}

void renderFeatureHub(float cx0,float y,float slide){
    const float s=g_uiScale;
    const float x=cx0+14*s;
    const float totalW=652*s;
    const float gap=10*s;
    const float cardW=(totalW-gap)*0.5f;
    const float cardH=58*s;
    const float top=y+58*s+slide;
    struct Item{const char* id;const char* title;const char* desc;int icon;int section;int parent;};
    const Item items[]={
        {"aim_main","Aimbot","Aim behavior and limits",0,101,1},
        {"aim_target","Target Selection","Target filters and priority",3,102,1},
        {"aim_assist","Assists","RCS, prediction and FOV",4,103,1},
        {"aim_parts","Body Parts","Selectable hit regions",5,104,1},
        {"vis_esp","ESP","Boxes and enemy indicators",1,201,2},
        {"vis_overlays","Overlays","Health, armor and labels",2,202,2},
        {"vis_style","ESP Style","Line width, fill and colors",5,203,2},
        {"world_fog","Fog","Mode, density, range and tint",4,401,4},
        {"world_ambient","Ambient","Ambient light and intensity",4,402,4},
        {"world_shadows","Shadows","Shadow distance control",4,403,4},
        {"misc_tools","Utilities","Watermark and editor tools",7,301,3},
        {"interface_menu","Interface","Menu scale and transparency",7,501,5},
        {"interface_theme","Theme","Accent and text colors",5,502,5},
    };
    int visible=0;
    for(int i=0;i<int(sizeof(items)/sizeof(items[0]));++i){
        if(items[i].parent!=g_section)continue;
        const int row=visible/2,col=visible%2;
        const ImVec2 pos(x+col*(cardW+gap),top+row*(cardH+8*s));
        renderFeatureCard(items[i].id,items[i].title,items[i].desc,items[i].icon,items[i].section,pos,ImVec2(cardW,cardH));
        ++visible;
    }
}

void renderSettingsPage(int section){
    const float s=g_uiScale;
    const float width=ImGui::GetContentRegionAvail().x;
    auto gap=[&](){ImGui::Dummy(ImVec2(1.0f,5.0f*s));};
    auto heading=[&](const char* label){ImGui::TextColored(g_appearance.accent,"%s",label);ImGui::Dummy(ImVec2(1.0f,4.0f*s));};
    if(section>=101&&section<=104){
        auto c=lemming::aim::configuration();auto& cfg=c.normal;
        if(section==101){heading("AIMBOT");uiToggle("Enable Aimbot",cfg.enabled);uiToggle("Pause while menu is open",c.pauseInMenu);uiSlider("fov","FOV",cfg.fov,0.1f,180.0f,"%.1f deg");uiSlider("max_dist","Max distance",cfg.maxDistance,1.0f,500.0f,"%.0f m");uiSlider("smoothing","Smoothness",cfg.smoothing,0.0f,1.0f,"%.2f");}
        else if(section==102){heading("TARGET SELECTION");uiToggle("Only while attacking",cfg.onAttack);uiToggle("Lock target",cfg.lockTarget);uiToggle("Visible check",cfg.visibleCheck);static const char* pri[] = {"Angle","Distance","HP"};uiCombo("priority","Priority",cfg.priority,pri,3);}
        else if(section==103){heading("ASSISTS");uiToggle("Recoil control (RCS)",cfg.rcs);if(cfg.rcs)uiSlider("rcs_strength","RCS strength",cfg.rcsStrength,0.0f,100.0f,"%.0f%%");uiToggle("Show FOV",cfg.showFov);uiToggle("Prediction",cfg.prediction);if(cfg.prediction)uiSlider("pred_ms","Prediction lead",cfg.predictionMs,0.0f,100.0f,"%.0f ms");}
        else {heading("BODY PARTS");for(int i=0;i<8;++i){const bool selected=(cfg.bodies&(1u<<i))!=0;if(uiChoiceButton(lemming::aim::kBodyNames[i],lemming::aim::kBodyNames[i],selected,ImVec2(width-4*s,36*s)))cfg.bodies^=(1u<<i);gap();}uiToggle("Multi-points",cfg.multipoints);if(cfg.multipoints)uiSlider("point_scale","Multi-point scale",cfg.pointScale,0.0f,0.9f,"%.2f");}
        c.normal=cfg;lemming::aim::publish(c);return;
    }
    if(section>=201&&section<=203){
        if(section==201){heading("ESP");bool en=g_espBoxesEnabled.load();if(uiToggle("Enable enemy ESP",en))g_espBoxesEnabled.store(en);uiToggle("Bounding box",g_appearance.box);uiToggle("Rounded corners",g_appearance.rounded);uiToggle("Soft fill",g_appearance.fill);}
        else if(section==202){heading("OVERLAYS");uiToggle("Health",g_appearance.health);uiToggle("Armor",g_appearance.armor);uiToggle("Weapon label",g_appearance.weapon);uiToggle("Ammo",g_appearance.ammo);uiColorRow("Text color",g_appearance.textColor);}
        else {heading("ESP STYLE");static const char* styles[]={"Full box","Corner box"};int style=g_appearance.boxStyle;if(uiCombo("box_style","Box style",style,styles,2))g_appearance.boxStyle=style;uiSlider("thickness","Line width",g_appearance.lineWidth,1.0f,4.0f,"%.1f px");uiSlider("fill_intensity","Fill intensity",g_appearance.fillIntensity,0.02f,0.30f,"%.2f");uiSlider("corner_radius","Corner radius",g_appearance.cornerRadius,0.0f,18.0f,"%.0f px");uiColorRow("Box color",g_appearance.boxColor);uiColorRow("Health color",g_appearance.healthColor);uiColorRow("Armor color",g_appearance.armorColor);}
        return;
    }
    if(section==401){
        auto& f=xim::features::world_mod::config().fog;heading("FOG");uiToggle("Enable Fog",f.enabled);static const char* modes[]={"Linear","Exponential","Exponential squared"};uiCombo("fog_mode","Mode",f.mode,modes,3);uiSlider("fog_density","Density",f.density,0.0f,1.0f,"%.3f");uiSlider("fog_start","Start distance",f.start,0.0f,1000.0f,"%.1f");uiSlider("fog_end","End distance",f.end,0.1f,5000.0f,"%.1f");if(f.end<f.start+0.1f)f.end=f.start+0.1f;ImVec4 fogColor{f.color[0],f.color[1],f.color[2],f.color[3]};uiColorRow("Fog color",fogColor);f.color[0]=fogColor.x;f.color[1]=fogColor.y;f.color[2]=fogColor.z;f.color[3]=fogColor.w;return;
    }
    if(section==402){auto& a=xim::features::world_mod::config().ambient;heading("AMBIENT LIGHT");uiToggle("Enable Ambient",a.enabled);uiSlider("ambient_intensity","Intensity",a.intensity,0.0f,3.0f,"%.2f");ImVec4 ambientColor{a.color[0],a.color[1],a.color[2],a.color[3]};uiColorRow("Ambient color",ambientColor);a.color[0]=ambientColor.x;a.color[1]=ambientColor.y;a.color[2]=ambientColor.z;a.color[3]=ambientColor.w;return;}
    if(section==403){auto& world=xim::features::world_mod::config();heading("SHADOWS");uiToggle("Disable shadows",world.culling.no_shadows);ImGui::TextWrapped("Uses Unity RenderSettings shadow distance. Availability depends on the active game renderer.");return;}
    if(section==301){heading("UTILITIES");if(uiActionButton("open_esp_editor","Open ESP editor","Preview and tune the ESP appearance",5))g_espEditorOpen=true;ImGui::Dummy(ImVec2(1,12*s));if(uiActionButton("reset_watermark","Reset watermark position","Move the watermark to the bottom-right corner",7)){g_wm.x=-1.0f;g_wm.y=-1.0f;}return;}
    if(section==501){heading("INTERFACE");uiSliderInt("menu_scale","Menu scale",g_menuScale,1,5,"%d");ImGui::TextWrapped("The menu uses a translucent background. Settings windows share the same scale and accent color.");return;}
    if(section==502){heading("THEME");uiColorRow("Accent color",g_appearance.accent);uiColorRow("Text color",g_appearance.textColor);return;}
}

const char* settingsPageTitle(int section){
    switch(section){case 101:return "Aimbot";case 102:return "Target Selection";case 103:return "Assists";case 104:return "Body Parts";case 201:return "ESP";case 202:return "Overlays";case 203:return "ESP Style";case 301:return "Utilities";case 401:return "Fog";case 402:return "Ambient";case 403:return "Shadows";case 501:return "Interface";case 502:return "Theme";default:return "Settings";}
}

void renderFeatureSettingsWindow(){
    if(!g_settingsWindowOpen)return;
    const float s=g_uiScale;
    const auto display=ImGui::GetIO().DisplaySize;
    const ImVec2 size(std::min(520.0f*s,display.x-16.0f*s),std::min(590.0f*s,display.y-16.0f*s));
    if(g_settingsWindowPos.x<0||g_settingsWindowPos.y<0)g_settingsWindowPos=ImVec2((display.x-size.x)*0.5f,(display.y-size.y)*0.5f);
    g_settingsWindowPos.x=std::clamp(g_settingsWindowPos.x,0.0f,std::max(0.0f,display.x-size.x));
    g_settingsWindowPos.y=std::clamp(g_settingsWindowPos.y,0.0f,std::max(0.0f,display.y-size.y));
    ImGui::SetNextWindowPos(g_settingsWindowPos,ImGuiCond_Always);
    ImGui::SetNextWindowSize(size,ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,9.0f*s);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(12.0f*s,10.0f*s));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,1.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,uiColor(ImVec4(ximira_theme::kWindowBg.x,ximira_theme::kWindowBg.y,ximira_theme::kWindowBg.z,0.94f)));
    ImGui::PushStyleColor(ImGuiCol_Border,uiColor(g_appearance.accent,0.65f));
    ImGuiWindowFlags flags=ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoScrollbar;
    if(ImGui::Begin("##ximira_feature_settings",&g_settingsWindowOpen,flags)){
        const ImVec2 wp=ImGui::GetWindowPos();const ImVec2 ws=ImGui::GetWindowSize();auto* d=ImGui::GetWindowDrawList();
        d->AddRectFilled(wp,ImVec2(wp.x+ws.x,wp.y+43*s),uiColor(ImVec4(ximira_theme::kPanelBg.x,ximira_theme::kPanelBg.y,ximira_theme::kPanelBg.z,0.98f)),9*s,ImDrawFlags_RoundCornersTop);
        d->AddRectFilled(ImVec2(wp.x+10*s,wp.y+41*s),ImVec2(wp.x+ws.x-10*s,wp.y+42*s),uiColor(g_appearance.accent,0.8f),1*s);
        ImGui::SetCursorScreenPos(ImVec2(wp.x+12*s,wp.y+4*s));
        ImGui::InvisibleButton("##settings_drag_bar",ImVec2(ws.x-60*s,34*s));
        const bool active=ImGui::IsItemActive();
        if(active&&!g_settingsDrag){g_settingsDrag=true;g_settingsDragOffset=ImVec2(ImGui::GetIO().MousePos.x-wp.x,ImGui::GetIO().MousePos.y-wp.y);}
        if(g_settingsDrag){if(ImGui::GetIO().MouseDown[0]){g_settingsWindowPos=ImVec2(ImGui::GetIO().MousePos.x-g_settingsDragOffset.x,ImGui::GetIO().MousePos.y-g_settingsDragOffset.y);ImGui::SetWindowPos(g_settingsWindowPos);}else g_settingsDrag=false;}
        uiTextLeft(d,ImVec2(wp.x+16*s,wp.y+12*s),"X",uiColor(g_appearance.accent),16*s,g_uiTitleFont);
        uiTextLeft(d,ImVec2(wp.x+34*s,wp.y+12*s),settingsPageTitle(g_settingsSection),uiColor(ximira_theme::kText),16*s,g_uiTitleFont);
        ImGui::SetCursorScreenPos(ImVec2(wp.x+ws.x-34*s,wp.y+8*s));
        if(ImGui::InvisibleButton("##settings_close",ImVec2(22*s,24*s)))g_settingsWindowOpen=false;
        const ImVec2 closeA=ImGui::GetItemRectMin();
        d->AddLine(uiAdd(closeA,6*s,6*s),uiAdd(closeA,16*s,16*s),uiColor(ximira_theme::kTextMuted),1.7f*s);
        d->AddLine(uiAdd(closeA,16*s,6*s),uiAdd(closeA,6*s,16*s),uiColor(ximira_theme::kTextMuted),1.7f*s);
        ImGui::SetCursorScreenPos(ImVec2(wp.x+14*s,wp.y+53*s));
        ImGui::BeginChild("##settings_page_scroll",ImVec2(ws.x-28*s,ws.y-65*s),ImGuiChildFlags_None,ImGuiWindowFlags_None);
        renderSettingsPage(g_settingsSection);
        ImGui::EndChild();
    }
    ImGui::End();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(3);
}

// ─── MAIN ──────────────────────────────────────────────────────────────────
struct MenuPos { float x=-1.0f, y=-1.0f; bool drag=false; float dragOffX=0, dragOffY=0; };
MenuPos g_menuPos{};

void renderMinimalMenu(){
    auto& io=ImGui::GetIO();

    // menu scale: 1..5 → 0.60..1.40
    float autoScale=std::min({io.DisplaySize.y/1080.0f,io.DisplaySize.x/1440.0f,
                              io.DisplaySize.y/760.0f,1.25f});
    autoScale=std::max(0.25f,autoScale);
    const float userMul=0.60f+float(std::clamp(g_menuScale,1,5)-1)*0.20f;
    g_uiScale=autoScale*userMul;
    io.FontGlobalScale=g_uiScale;

    // World settings are applied once through xim::features::world_mod in drawOverlay.
    // Do not run the legacy direct-RVA path here: it can restore the original fog colour
    // after the active RenderSettings setter has applied the user's selection.

    renderEspBoxes();
    renderAngleFov();
    renderWatermark();
    renderSplash();

    lemming::aim::g_menuOpen.store(g_menuOpen.load(),std::memory_order_release);

    const bool open=g_menuOpen.load(std::memory_order_relaxed);
    g_menuAnimation=uiEase(g_menuAnimation,open?1:0);
    if(g_menuAnimation<0.005f&&!open)return;

    g_pageAnimation=uiEase(g_pageAnimation,1,15);
    const float s=g_uiScale;
    const float railW=150*s;
    const float contentW=680*s;
    const float totalW=railW+contentW;
    const float h=600*s;

    // default: center
    if(g_menuPos.x<0){
        g_menuPos.x=std::floor((io.DisplaySize.x-totalW)*.5f);
        g_menuPos.y=std::floor((io.DisplaySize.y-h)*.5f);
    }
    const float x=g_menuPos.x;
    const float y=g_menuPos.y+(1-g_menuAnimation)*10*s;

    auto flags=ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_NoMove|
               ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoBackground;
    if(!open)flags|=ImGuiWindowFlags_NoInputs;

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha,g_menuAnimation);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(0,0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,ImVec2(0,0));

    // RAIL
    ImGui::SetNextWindowPos(ImVec2(x,y),ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(railW,h),ImGuiCond_Always);
    ImGui::Begin("##ximira_rail",nullptr,flags);
    {
        auto* d=ImGui::GetWindowDrawList();
        d->AddRectFilled(ImVec2(x,y),ImVec2(x+railW,y+h),uiColor(ImVec4(ximira_theme::kPanelBg.x,ximira_theme::kPanelBg.y,ximira_theme::kPanelBg.z,0.94f)),6*s);
        d->AddRect(ImVec2(x,y),ImVec2(x+railW,y+h),uiColor(ximira_theme::kBorder),6*s,0,1.0f);

        // DRAG на логотипе rail
        ImGui::SetCursorScreenPos(ImVec2(x+16*s,y+16*s));
        ImGui::InvisibleButton("##rail_drag",ImVec2(railW-32*s,40*s));
        const bool railActive=ImGui::IsItemActive();
        const bool railHovered=ImGui::IsItemHovered();
        if(railActive&&!g_menuPos.drag){
            g_menuPos.drag=true;
            g_menuPos.dragOffX=io.MousePos.x-x;
            g_menuPos.dragOffY=io.MousePos.y-y;
        }
        if(g_menuPos.drag){
            if(io.MouseDown[0]){
                g_menuPos.x=io.MousePos.x-g_menuPos.dragOffX;
                g_menuPos.y=io.MousePos.y-g_menuPos.dragOffY;
                if(g_menuPos.x<0)g_menuPos.x=0;
                if(g_menuPos.y<0)g_menuPos.y=0;
                if(g_menuPos.x+totalW>io.DisplaySize.x)g_menuPos.x=io.DisplaySize.x-totalW;
                if(g_menuPos.y+h>io.DisplaySize.y)g_menuPos.y=io.DisplaySize.y-h;
            }else{
                g_menuPos.drag=false;
            }
        }

        const ImVec2 logoP=ImVec2(x+16*s,y+16*s);
        const float logoSize=20*s;
        d->AddLine(uiAdd(logoP,0,0),uiAdd(logoP,logoSize,logoSize),
            uiColor(g_appearance.accent),2.2f*s);
        d->AddLine(uiAdd(logoP,logoSize,0),uiAdd(logoP,0,logoSize),
            uiColor(g_appearance.accent),2.2f*s);
        uiTextLeft(d,uiAdd(logoP,logoSize+10*s,2*s),"XIMIRA",
            uiColor(ximira_theme::kText),18*s,g_uiTitleFont);
        uiTextLeft(d,uiAdd(logoP,logoSize+10*s,22*s),"PREMIUM",uiColor(ximira_theme::kTextMuted),9*s,g_uiSmallFont);
        const ImVec2 onDot=ImVec2(x+railW-20*s,y+26*s);
        d->AddCircleFilled(onDot,3.5f*s,uiColor(ximira_theme::kOnline),12);
        d->AddCircleFilled(onDot,6.5f*s,uiColor(ximira_theme::kOnline,0.20f),12);

        d->AddLine(ImVec2(x+10*s,y+48*s),ImVec2(x+railW-10*s,y+48*s),
            uiColor(ximira_theme::kBorderDim),1.0f);

        struct Tab{const char* id;const char* label;int icon;int value;};
        const Tab tabs[5]={{"aimbot","Aimbot",0,1},{"visuals","Visuals",1,2},
                           {"misc","Misc",2,3},{"world","World",4,4},
                           {"settings","Settings",7,5}};
        for(int i=0;i<5;++i){
            ImGui::SetCursorScreenPos(ImVec2(x,y+58*s+float(i)*46*s));
            if(uiRailIcon(tabs[i].id,tabs[i].icon,g_section==tabs[i].value,railW-16*s)){
                if(g_section!=tabs[i].value){g_section=tabs[i].value;g_pageAnimation=0;}
            }
        }

        const ImVec2 tg=ImVec2(x+14*s,y+h-32*s);
        d->AddTriangleFilled(uiAdd(tg,0,2*s),uiAdd(tg,10*s,7*s),uiAdd(tg,0,12*s),
            uiColor(g_appearance.accent,0.9f));
        uiTextLeft(d,uiAdd(tg,16*s,0),"@skeetcod",
            uiColor(ximira_theme::kTextDim),13*s,g_uiSmallFont);
    }
    ImGui::End();

    // CONTENT
    ImGui::SetNextWindowPos(ImVec2(x+railW,y),ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(contentW,h),ImGuiCond_Always);
    ImGui::Begin("##ximira_content",nullptr,flags);
    {
        auto* d=ImGui::GetWindowDrawList();
        d->AddRectFilled(ImVec2(x+railW,y),ImVec2(x+totalW,y+h),
            uiColor(ImVec4(ximira_theme::kWindowBg.x,ximira_theme::kWindowBg.y,ximira_theme::kWindowBg.z,0.88f)),6*s);
        d->AddRect(ImVec2(x+railW,y),ImVec2(x+totalW,y+h),
            uiColor(ximira_theme::kBorder),6*s,0,1.0f);
        d->AddRectFilled(ImVec2(x+railW+1,y+1),ImVec2(x+totalW-1,y+2),
            uiColor(g_appearance.accent,0.55f),0.0f);

        const float cx0=x+railW;
        const char* rightName="";
        switch(g_section){
            case 1: rightName="Aimbot";break;
            case 2: rightName="Visuals";break;
            case 3: rightName="Misc";break;
            case 4: rightName="World";break;
            case 5: rightName="Settings";break;
        }
        uiTextLeft(d,ImVec2(cx0+18*s,y+14*s),"ximira",
            uiColor(ximira_theme::kText),18*s,g_uiTitleFont);
        const float rtW=uiTextWidth(rightName,14*s,g_uiSmallFont);
        uiTextLeft(d,ImVec2(cx0+contentW-rtW-18*s,y+18*s),rightName,
            uiColor(ximira_theme::kTextMuted),14*s,g_uiSmallFont);
        d->AddLine(ImVec2(cx0+14*s,y+42*s),ImVec2(cx0+contentW-14*s,y+42*s),
            uiColor(ximira_theme::kBorderDim),1.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha,g_menuAnimation*(0.2f+0.8f*g_pageAnimation));
        const float slide=(1-g_pageAnimation)*8*s;

        renderFeatureHub(cx0,y,slide);

        ImGui::PopStyleVar();
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
    if(open&&g_espEditorOpen)renderEspEditor();
    if(open&&g_settingsWindowOpen)renderFeatureSettingsWindow();
}