using UnityEngine;

/// <summary>
/// Enables/disables the named passes of Zenin/CharacterMultipass.
/// Attach to the same GameObject as the Renderer.
/// </summary>
[DisallowMultipleComponent]
public sealed class CharacterMultipassController : MonoBehaviour
{
    public enum EffectType
    {
        Solid = 0, Shaded = 1, Glow = 2, Iridescent = 3, Water = 4,
        Glossy = 5, Wireframe = 6, Glass = 7, Blur = 8, Crystal = 9,
        Lava = 10, Ice = 11, Matrix = 12
    }

    [SerializeField] private Renderer targetRenderer;
    [SerializeField] private EffectType effect = EffectType.Solid;
    [SerializeField] private Color tint = new Color(0.2f, 0.8f, 1f, 1f);
    [SerializeField] private Color glowColor = new Color(0.2f, 0.8f, 1f, 1f);
    [SerializeField, Range(0f, 8f)] private float glowIntensity = 2f;
    [SerializeField, Range(0f, 0.08f)] private float outlineWidth = 0.015f;
    [SerializeField] private Color wireColor = new Color(0.2f, 1f, 0.8f, 1f);
    [SerializeField, Range(0.5f, 4f)] private float wireWidth = 1.2f;
    [SerializeField, Range(0f, 4f)] private float speed = 1f;
    [SerializeField, Range(0f, 4f)] private float strength = 1f;
    [SerializeField, Range(0f, 1f)] private float roughness = 0.35f;
    [SerializeField] private bool useUnscaledTime;

    private Material runtimeMaterial;

    private const string MainPass = "CHARACTER_MAIN";
    private const string GlowPass = "CHARACTER_GLOW";
    private const string WirePass = "CHARACTER_WIREFRAME";

    private static readonly int TypeId = Shader.PropertyToID("_EffectType");
    private static readonly int TimeId = Shader.PropertyToID("_TimeValue");
    private static readonly int ColorId = Shader.PropertyToID("_Color");
    private static readonly int GlowColorId = Shader.PropertyToID("_GlowColor");
    private static readonly int GlowIntensityId = Shader.PropertyToID("_GlowIntensity");
    private static readonly int OutlineWidthId = Shader.PropertyToID("_OutlineWidth");
    private static readonly int WireColorId = Shader.PropertyToID("_WireColor");
    private static readonly int WireWidthId = Shader.PropertyToID("_WireWidth");
    private static readonly int SpeedId = Shader.PropertyToID("_Speed");
    private static readonly int StrengthId = Shader.PropertyToID("_Strength");
    private static readonly int RoughnessId = Shader.PropertyToID("_Roughness");

    private void Awake()
    {
        if (!targetRenderer) targetRenderer = GetComponent<Renderer>();
        if (!targetRenderer)
        {
            Debug.LogError("CharacterMultipassController: Renderer not found.", this);
            enabled = false;
            return;
        }

        // renderer.material creates an instance, avoiding edits to the shared asset.
        runtimeMaterial = targetRenderer.material;
        if (runtimeMaterial.shader.name != "Zenin/CharacterMultipass")
            Debug.LogWarning("Expected shader 'Zenin/CharacterMultipass'.", this);

        ApplySettings();
    }

    private void Update()
    {
        if (!runtimeMaterial) return;
        runtimeMaterial.SetFloat(TimeId, useUnscaledTime ? Time.unscaledTime : Time.time);
    }

    public void SetEffect(EffectType newEffect)
    {
        effect = newEffect;
        ApplyPasses();
        if (runtimeMaterial) runtimeMaterial.SetFloat(TypeId, (float)effect);
    }

    public void SetGlowPass(bool enabled)
    {
        if (runtimeMaterial) runtimeMaterial.SetShaderPassEnabled(GlowPass, enabled);
    }

    public void SetWireframePass(bool enabled)
    {
        if (runtimeMaterial) runtimeMaterial.SetShaderPassEnabled(WirePass, enabled);
    }

    private void ApplySettings()
    {
        runtimeMaterial.SetColor(ColorId, tint);
        runtimeMaterial.SetColor(GlowColorId, glowColor);
        runtimeMaterial.SetFloat(GlowIntensityId, glowIntensity);
        runtimeMaterial.SetFloat(OutlineWidthId, outlineWidth);
        runtimeMaterial.SetColor(WireColorId, wireColor);
        runtimeMaterial.SetFloat(WireWidthId, wireWidth);
        runtimeMaterial.SetFloat(SpeedId, speed);
        runtimeMaterial.SetFloat(StrengthId, strength);
        runtimeMaterial.SetFloat(RoughnessId, roughness);
        runtimeMaterial.SetFloat(TypeId, (float)effect);
        runtimeMaterial.SetFloat(TimeId, useUnscaledTime ? Time.unscaledTime : Time.time);
        ApplyPasses();
    }

    private void ApplyPasses()
    {
        if (!runtimeMaterial) return;

        // Main surface always draws. Glow and wire passes are enabled only for their modes.
        runtimeMaterial.SetShaderPassEnabled(MainPass, true);
        runtimeMaterial.SetShaderPassEnabled(GlowPass, effect == EffectType.Glow);
        runtimeMaterial.SetShaderPassEnabled(WirePass, effect == EffectType.Wireframe);
    }

    private void OnDestroy()
    {
        if (runtimeMaterial) Destroy(runtimeMaterial);
    }
}
