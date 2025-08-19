using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public struct sound_setting_data
{
    public float master;
    public float bgm;
    public float sfx;
}

public struct ohter_setting_data
{
    public bool full_screen;
}


public class LoadSettingDatas : MonoBehaviour
{
    [SerializeField] Slider master;
    [SerializeField] Slider bgm;
    [SerializeField] Slider sfx;
    [SerializeField] Toggle full_screen;
    private SaveLoad sl;

    public void ApplyFullScreenSettings(bool isFullScreen)
    {
        Debug.Log($"Is full ? {isFullScreen}");
        var setting = new ohter_setting_data
        {
            full_screen = isFullScreen
        };
        sl.Save(setting);
        Screen.SetResolution(1920,1080, isFullScreen);
    }


    private void Awake()
    {
        full_screen.onValueChanged.AddListener(ApplyFullScreenSettings);
    }

    private void Start()
    {
        sl = FindObjectOfType<SaveLoad>();
        var data = sl.Load<sound_setting_data>();
        var fullscreenSettingData = sl.Load<ohter_setting_data>();

        full_screen.isOn = fullscreenSettingData?.full_screen ?? true;
        
        if (!data.HasValue) return;
        master.value = data.Value.master;
        bgm.value = data.Value.bgm;
        sfx.value = data.Value.sfx;
        
        
    }
}
