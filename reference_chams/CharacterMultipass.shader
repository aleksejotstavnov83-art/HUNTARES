Shader "Zenin/CharacterMultipass"
{
    Properties
    {
        _MainTex ("Albedo", 2D) = "white" {}
        _Color ("Effect Tint", Color) = (0.2,0.8,1,1)
        _GlowColor ("Glow Color", Color) = (0.2,0.8,1,1)
        _GlowIntensity ("Glow Intensity", Range(0,8)) = 2
        _OutlineWidth ("Glow Outline Width", Range(0,0.08)) = 0.015
        _WireColor ("Wire Color", Color) = (0.2,1,0.8,1)
        _WireWidth ("Wire Width", Range(0.5,4)) = 1.2
        _FillColor ("Wire Fill", Color) = (0.02,0.05,0.08,0.15)
        _EffectColor2 ("Secondary Color", Color) = (1,0.25,0.05,1)
        _EffectColor3 ("Tertiary Color", Color) = (0.25,1,0.55,1)
        _EffectType ("uType (0-12)", Float) = 0
        _TimeValue ("uTime", Float) = 0
        _Speed ("Speed", Range(0,4)) = 1
        _Strength ("Strength", Range(0,4)) = 1
        _Roughness ("Roughness", Range(0,1)) = 0.35
        _Opacity ("Opacity", Range(0,1)) = 1
        _Overlay ("Overlay albedo", Range(0,1)) = 0
        _Distortion ("Refraction distortion", Range(0,0.08)) = 0.015
        _Heat ("Lava heat", Range(0,1)) = 0.8
        _Frost ("Ice frost", Range(0,1)) = 0.6
        _Cracks ("Ice cracks", Range(0,1)) = 0.5
        _MatrixDensity ("Matrix density", Range(4,64)) = 24
        _BloomEnabled ("Bloom pass enabled", Float) = 0
        _WireEnabled ("Wire pass enabled", Float) = 0
    }

    SubShader
    {
        Tags { "Queue"="Transparent" "RenderType"="Transparent" }
        LOD 300

        // MAIN: x-ray character surface. This intentionally ignores scene depth.
        Pass
        {
            Name "CHARACTER_MAIN"
            Tags { "LightMode"="Always" }
            ZTest Always
            ZWrite Off
            Cull Back
            Blend SrcAlpha OneMinusSrcAlpha

            CGPROGRAM
            #pragma target 3.0
            #pragma vertex vertMain
            #pragma fragment fragMain
            #include "UnityCG.cginc"

            sampler2D _MainTex;
            float4 _MainTex_ST, _Color, _EffectColor2, _EffectColor3;
            float _EffectType, _TimeValue, _Speed, _Strength, _Roughness;
            float _Opacity, _Overlay, _Distortion, _Heat, _Frost, _Cracks, _MatrixDensity;

            struct appdata { float4 vertex:POSITION; float3 normal:NORMAL; float2 uv:TEXCOORD0; float3 bary:TEXCOORD1; };
            struct v2f { float4 pos:SV_POSITION; float2 uv:TEXCOORD0; float3 worldPos:TEXCOORD1; float3 worldNormal:TEXCOORD2; float3 bary:TEXCOORD3; float4 screen:TEXCOORD4; };

            v2f vertMain(appdata v)
            {
                v2f o;
                o.pos=UnityObjectToClipPos(v.vertex);
                o.uv=TRANSFORM_TEX(v.uv,_MainTex);
                o.worldPos=mul(unity_ObjectToWorld,v.vertex).xyz;
                o.worldNormal=UnityObjectToWorldNormal(v.normal);
                o.bary=v.bary;
                o.screen=ComputeScreenPos(o.pos);
                return o;
            }

            float hash21(float2 p)
            {
                p=frac(p*float2(123.34,456.21)); p+=dot(p,p+45.32);
                return frac(p.x*p.y);
            }
            float noise2(float2 p)
            {
                float2 i=floor(p),f=frac(p); f=f*f*(3-2*f);
                return lerp(lerp(hash21(i),hash21(i+float2(1,0)),f.x),
                            lerp(hash21(i+float2(0,1)),hash21(i+1),f.x),f.y);
            }
            float3 rainbow(float x) { return 0.5+0.5*cos(6.28318*(x+float3(0,0.3333,0.6667))); }

            fixed4 fragMain(v2f i):SV_Target
            {
                float t=_TimeValue;
                float4 tex=tex2D(_MainTex,i.uv);
                float4 baseCol=tex*_Color;
                float3 N=normalize(i.worldNormal);
                float3 V=normalize(_WorldSpaceCameraPos-i.worldPos);
                float3 L=normalize(float3(-0.35,0.75,0.55));
                float ndl=saturate(dot(N,L)), ndv=saturate(dot(N,V));
                float fres=pow(1-ndv,3);
                float3 c=baseCol.rgb;
                float a=baseCol.a*_Opacity;

                // 0 Solid
                if(_EffectType<0.5) c=lerp(tex.rgb,baseCol.rgb,_Overlay);
                // 1 Shaded
                else if(_EffectType<1.5)
                {
                    float3 H=normalize(L+V);
                    float spec=pow(saturate(dot(N,H)),lerp(12,96,1-_Roughness));
                    c=baseCol.rgb*(0.22+0.78*ndl)+spec*_Strength*0.35;
                }
                // 2 Glow body (outline is the separate GLOW pass)
                else if(_EffectType<2.5)
                {
                    float g=fres*(0.7+_Strength)+pow(ndv,8)*0.15;
                    c=baseCol.rgb*(0.35+g*1.4);
                    a*=saturate(0.2+g);
                }
                // 3 Iridescent
                else if(_EffectType<3.5)
                {
                    float phase=ndv*7+dot(i.worldPos,float3(.05,.12,.04))+t*.35;
                    c=lerp(baseCol.rgb,rainbow(phase)*baseCol.rgb*1.5,saturate(.35+fres));
                    c*=.55+.45*ndl;
                }
                // 4 Water flow
                else if(_EffectType<4.5)
                {
                    float n=noise2(i.uv*9+float2(t*_Speed,-t*_Speed*.7));
                    float bands=pow(saturate(.5+.5*sin((i.uv.x+i.uv.y)*30-t*3*_Speed)),5);
                    float foam=saturate(n*.55+bands);
                    c=lerp(baseCol.rgb*.12,baseCol.rgb*(.55+foam),.85)+foam*.35;
                    c+=pow(saturate(dot(N,normalize(L+V))),80)*.7;
                }
                // 5 Glossy
                else if(_EffectType<5.5)
                {
                    float3 H=normalize(L+V);
                    float spec=pow(saturate(dot(N,H)),lerp(24,160,1-_Roughness));
                    c=baseCol.rgb*(.12+ndl*.18)+spec*_Strength+fres*.25;
                }
                // 6 Wireframe base fill; actual lines are in separate WIREFRAME pass.
                else if(_EffectType<6.5) c=baseCol.rgb*.12;
                // 7 Glass approximation; use a grab/refraction renderer feature for true scene refraction.
                else if(_EffectType<7.5)
                {
                    c=lerp(baseCol.rgb,_EffectColor2.rgb,.22)+fres*_Color.rgb;
                    a*=saturate(.22+fres*.65);
                }
                // 8 Blur approximation (screen-space blur requires a grab texture/post effect).
                else if(_EffectType<8.5)
                {
                    c=baseCol.rgb*(.65+fres*.35);
                    a*=.72;
                }
                // 9 Crystal
                else if(_EffectType<9.5)
                {
                    float n=noise2(floor(i.uv*lerp(8,25,_Strength))+.17);
                    c=lerp(baseCol.rgb*.4,rainbow(n+fres*.2)*baseCol.rgb,0.8);
                    c+=pow(ndv,14)*.5;
                }
                // 10 Lava
                else if(_EffectType<10.5)
                {
                    float n=noise2(i.uv*lerp(6,20,_Strength)+float2(t*.25,-t*.18));
                    float vein=1-smoothstep(.025,.15,abs(n-.5));
                    float heat=saturate(_Heat*(vein*.9+n*.25));
                    c=lerp(baseCol.rgb*.1,_EffectColor2.rgb*2.2,heat)+vein*_EffectColor2.rgb;
                }
                // 11 Ice
                else if(_EffectType<11.5)
                {
                    float n=noise2(i.uv*lerp(8,32,_Cracks));
                    float crack=(1-smoothstep(.02,.09,abs(n-.5)))*_Cracks;
                    float frost=saturate(_Frost*(.4+fres+n*.35));
                    c=lerp(baseCol.rgb*.3,float3(.72,.9,1),frost);
                    c=lerp(c,float3(.95,.99,1),crack*.8);
                    c+=pow(saturate(dot(N,normalize(L+V))),80)*.45;
                }
                // 12 Matrix rain
                else
                {
                    float2 grid=i.uv*_MatrixDensity;
                    float2 cell=floor(grid), local=frac(grid);
                    float trail=frac(t*_Speed+hash21(float2(cell.x,0))*4-cell.y*.08);
                    float glyph=step(.56,hash21(floor(local*float2(4,6))+cell+floor(t*2)));
                    float rain=saturate((1-trail)*2.5)*glyph;
                    c=_Color.rgb*(.05+rain*1.7)+float3(.15,1,.3)*rain*.8;
                    a=saturate(_Opacity*(.06+rain));
                }

                return fixed4(c,saturate(a));
            }
            ENDCG
        }

        // GLOW: inverted hull outline rendered additively. Bloom post-processing makes it bloom outward.
        Pass
        {
            Name "CHARACTER_GLOW"
            Tags { "LightMode"="Always" }
            ZTest Always
            ZWrite Off
            Cull Front
            Blend One One

            CGPROGRAM
            #pragma target 3.0
            #pragma vertex vertGlow
            #pragma fragment fragGlow
            #include "UnityCG.cginc"
            float4 _GlowColor;
            float _GlowIntensity, _OutlineWidth, _EffectType;
            struct appdata { float4 vertex:POSITION; float3 normal:NORMAL; };
            struct v2f { float4 pos:SV_POSITION; };
            v2f vertGlow(appdata v)
            {
                v2f o;
                float3 expanded=v.vertex.xyz+normalize(v.normal)*_OutlineWidth;
                o.pos=UnityObjectToClipPos(float4(expanded,1));
                return o;
            }
            fixed4 fragGlow(v2f i):SV_Target
            {
                // Glow outline is active only for Glow mode; controller also toggles this pass.
                clip(2.5-_EffectType);
                clip(_EffectType-1.5);
                return fixed4(_GlowColor.rgb*_GlowIntensity,_GlowColor.a);
            }
            ENDCG
        }

        // WIREFRAME: requires barycentric coordinates in TEXCOORD1 (one-hot per triangle vertex).
        Pass
        {
            Name "CHARACTER_WIREFRAME"
            Tags { "LightMode"="Always" }
            ZTest Always
            ZWrite Off
            Cull Off
            Blend SrcAlpha OneMinusSrcAlpha

            CGPROGRAM
            #pragma target 3.0
            #pragma vertex vertWire
            #pragma fragment fragWire
            #include "UnityCG.cginc"
            float4 _WireColor, _FillColor;
            float _WireWidth, _EffectType;
            struct appdata { float4 vertex:POSITION; float3 bary:TEXCOORD1; };
            struct v2f { float4 pos:SV_POSITION; float3 bary:TEXCOORD0; };
            v2f vertWire(appdata v)
            {
                v2f o; o.pos=UnityObjectToClipPos(v.vertex); o.bary=v.bary; return o;
            }
            fixed4 fragWire(v2f i):SV_Target
            {
                clip(6.5-_EffectType);
                clip(_EffectType-5.5);
                float3 dx=ddx(i.bary), dy=ddy(i.bary);
                float3 fw=max(sqrt(dx*dx+dy*dy),1e-5);
                float3 edge=smoothstep(fw*(_WireWidth+0.5),fw*(_WireWidth-0.5),i.bary);
                float line=1-min(edge.x,min(edge.y,edge.z));
                clip(line-0.015);
                return fixed4(_WireColor.rgb,_WireColor.a*line);
            }
            ENDCG
        }
    }
    FallBack "Transparent/Diffuse"
}
