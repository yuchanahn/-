using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Audio;
using UnityEngine.UI;

public class SoundManager : MonoBehaviour
{
    [SerializeField] private AudioMixer masterMixer;
    [SerializeField] private Slider MasterSlider;
    [SerializeField] private Slider BGMSlider;
    [SerializeField] private Slider SFXSlider;

    private SaveLoad sl;
    private void Awake()
    {
        sl = FindObjectOfType<SaveLoad>();
        if(sl is null) return;
    }

    public void AudioControl()
    {
        float MasterVolume = MasterSlider.value;
        float BGMVolume = BGMSlider.value;
        float SFXVolume = SFXSlider.value;
        var data = new sound_setting_data
        {
            master = MasterVolume,
            bgm = BGMVolume,
            sfx = SFXVolume
        };
        sl.Save(data);
        
        if (MasterVolume <= -40f)
        {
            masterMixer.SetFloat("Master", -80);
        }
        else
        {
            masterMixer.SetFloat("Master", MasterVolume);
        }

        if (BGMVolume <= -40f)
        {
            masterMixer.SetFloat("BGM", -80);
        }
        else
        {
            masterMixer.SetFloat("BGM", BGMVolume);
        }

        if (SFXVolume <= -40f)
        {
            masterMixer.SetFloat("SFX", -80);
        }
        else
        {
            masterMixer.SetFloat("SFX", SFXVolume);
        }

    }

    public void ToggleAudioVolume()
    {
        AudioListener.volume = AudioListener.volume == 0 ? 1 : 0;
    }
}
