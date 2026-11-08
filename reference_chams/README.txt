ZENIN MULTIPASS UNITY PACKAGE

Files:
- CharacterMultipass.shader
- CharacterMultipassController.cs
- recovered_glsl_fragments.txt

Architecture:
- CHARACTER_MAIN: x-ray main surface, ZTest Always.
- CHARACTER_GLOW: additive inverted-hull outline, ZTest Always. For a soft halo/bloom beyond the outline, enable a Bloom post-processing effect and tune its threshold/intensity.
- CHARACTER_WIREFRAME: barycentric edge pass, ZTest Always.

Install:
1. Put shader under Assets/Shaders and controller under Assets/Scripts.
2. Create a material using shader Zenin/CharacterMultipass.
3. Assign material to MeshRenderer or SkinnedMeshRenderer.
4. Add CharacterMultipassController to the same object and assign Renderer.
5. For wireframe, mesh TEXCOORD1 must contain per-triangle barycentric coordinates: (1,0,0), (0,1,0), (0,0,1) at each triangle corner. Duplicate vertices at triangle boundaries if needed.
6. Main, glow and wireframe all use ZTest Always intentionally; they show through scene geometry. This also means they can draw over other characters/transparent objects depending on render order.
7. Glow pass is an emissive outline, not a complete bloom post-process. Use Unity Built-in Bloom/Post Processing Stack v2 or a compatible URP Bloom renderer feature for blur/spread.
8. Glass/Blur are stylized material approximations in this shader. True screen-space refraction/blur needs a GrabPass (Built-in only) or camera opaque texture / renderer feature (URP).

Notes:
- This shader uses Unity's normal mesh/skinned-mesh vertex pipeline; Unity handles SkinnedMeshRenderer skinning. It does not use the GLSL uBones texture path. That is preferable for standard Unity rigs and Android compatibility.
- The original upload is a mixed binary/text container. The extracted fragments are preserved separately; the Unity shader is a functional HLSL reimplementation of the named modes, not a verbatim source reconstruction.
- Target 3.0 / GLES3. Test on actual Android hardware. Built-in Render Pipeline shader; URP/HDRP require pass/render-pipeline adaptation.
