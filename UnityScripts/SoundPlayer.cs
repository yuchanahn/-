using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public struct BGMType
{
    public string name;
    public AudioClip clip;
    
}

public class SoundPlayer : MonoBehaviour
{
    public BGMType[] BGMList;
    private AudioSource audioSource;
    [SerializeField] private bool soundLoop;
    private void Start()
    {
        audioSource = GetComponent<AudioSource>();
        if (soundLoop)
            audioSource.loop = true;

        if (BGMList.Length > 0)
        {
            PlayBGMList();
        }
    }

    private void PlayBGMList()
    {
        for (int i = 0; i < BGMList.Length; i++)
        {
            audioSource.clip = BGMList[i].clip;
            audioSource.PlayOneShot(audioSource.clip);
        }
    }

}
