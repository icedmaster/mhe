using System;

namespace mhe
{
    public class LightSettings
    {
        public float float1;
        public float float2;
        public float float3;
        public float float4;
        public float float5;
        public float float6;
        public float float7;
        public float float8;
        public float shadowmapBias;
        public bool castShadows;
        public bool autoShadowConfiguration;

        public static LightSettings Deserialize(ByteStream stream)
        {
            var settings = new LightSettings();
            settings.float1 = Protocol.ParseFloat(stream);
            settings.float2 = Protocol.ParseFloat(stream);
            settings.float3 = Protocol.ParseFloat(stream);
            settings.float4 = Protocol.ParseFloat(stream);
            settings.float5 = Protocol.ParseFloat(stream);
            settings.float6 = Protocol.ParseFloat(stream);
            settings.float7 = Protocol.ParseFloat(stream);
            settings.float8 = Protocol.ParseFloat(stream);
            settings.shadowmapBias = Protocol.ParseFloat(stream);
            settings.castShadows = Protocol.ParseBool(stream);
            settings.autoShadowConfiguration = Protocol.ParseBool(stream);
            return settings;
        }
    }

    public class ShadingSettings
    {
        public Color diffuse;
        public Color specular;
        public float intensity;
        public float specularIntensity;

        public static ShadingSettings Deserialize(ByteStream stream)
        {
            var settings = new ShadingSettings();
            settings.diffuse = Protocol.ParseColor(stream);
            settings.specular = Protocol.ParseColor(stream);
            settings.intensity = Protocol.ParseFloat(stream);
            settings.specularIntensity = Protocol.ParseFloat(stream);
            return settings;
        }
    }

    public class Light
    {
        public enum Type
        {
            SPOT,
            OMNI,
            DIRECTIONAL
        }

        public static string TypeName = "light";

        public UInt16 id;

        [WidgetType(WidgetType.Type.TRANSFORM)]
        public TransformInstance transform;

        public AABBInstance aabb;

        [WidgetType(WidgetType.Type.CHECKBOX)]
        [FieldProtocolId(3)]
        public bool enabled;

        public Type type;

        public LightSettings lightSettings;
        public ShadingSettings shadingSettings;

        public static Light Deserialize(DataModel dataModel, ByteStream stream)
        {
            var light = new Light();
            light.id = Protocol.ParseUint16(stream);
            light.transform = new TransformInstance(dataModel, Protocol.ParseUint16(stream));
            var aabbId = Protocol.ParseUint16(stream);
            light.enabled = Protocol.ParseBool(stream);
            var type = Protocol.ParseInt(stream);
            light.type = (Light.Type)type;
            light.lightSettings = LightSettings.Deserialize(stream);
            light.shadingSettings = ShadingSettings.Deserialize(stream);
            return light;
        }
    }
}

