using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

[Serializable] public struct audio_data_t {
    public string name;
    public AudioClip clip;
}


public enum eAudioType {
    BGM,
    SFX,
}

public class AudioBinder : MonoBehaviour
{
    [SerializeField] List<audio_data_t> audios = new List<audio_data_t>(); 
    public Dictionary<string, AudioClip> audio_map = new Dictionary<string, AudioClip>();
    public Dictionary<eAudioType, AudioSource> audio_sourecs = new Dictionary<eAudioType, AudioSource>();

    private void Awake() {
        audio_sourecs[eAudioType.SFX] = GetComponentsInChildren<AudioSource>().ToList().First(x => x.name == eAudioType.SFX.ToString());
        audio_sourecs[eAudioType.BGM] = GetComponentsInChildren<AudioSource>().ToList().First(x => x.name == eAudioType.BGM.ToString());
        
        audios.ForEach(x => audio_map[x.name] = x.clip);
    }

    public void Play(string name, eAudioType type, bool loop = false)
    {
        if (!audio_map.ContainsKey(name)) return;
        audio_sourecs[type].clip = audio_map[name];
        audio_sourecs[type].loop = loop;
        audio_sourecs[type].Play();
    }

    public void PlayOneShot(string name, eAudioType type)
    {
        if (audio_map.ContainsKey(name)) audio_sourecs[type].PlayOneShot(audio_map[name]);
    }

    public void Stop(eAudioType type) {
        audio_sourecs[type].Stop();
        audio_sourecs[type].loop = false;
    }

    public void SetPlaySpeed(float speed, eAudioType type) {
        audio_sourecs[type].pitch = speed;
    }


    public void PlayBGM(string name_)
    {
        if (!audio_map.ContainsKey(name_)) return;
        audio_sourecs[eAudioType.BGM].clip = audio_map[name_];
        audio_sourecs[eAudioType.BGM].loop = true;
        audio_sourecs[eAudioType.BGM].Play();
    }
    
    public void PlaySFXOneShot(string name_)
    {
        if (audio_map.ContainsKey(name_)) audio_sourecs[eAudioType.SFX].PlayOneShot(audio_map[name_]);
    }
    
    public void PlaySFXLoop(string name_)
    {
        Play(name_, eAudioType.SFX, true);
    }
    
    public void StopSFX()
    {
        Stop(eAudioType.SFX);
    }

    public void StopAll()
    {
        Stop(eAudioType.BGM);
        Stop(eAudioType.SFX);
    }
}
