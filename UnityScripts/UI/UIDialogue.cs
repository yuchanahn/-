using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using TMPro;
using UnityEngine;


[Serializable] public struct dialogue_data_t {
    public string chracter_name;
    public string text;
}


public class UIDialogue : MonoBehaviour {
    public dialogue_data_t cur_data;
    public TextMeshProUGUI character_name;
    public TextMeshProUGUI msg;
    [SerializeField] float speed = 0.1f;
    Coroutine prev;

    public void OnChenaged() {
        msg.text = "";
        character_name.text = cur_data.chracter_name;
        if(prev is not null) StopCoroutine(prev);
        prev = StartCoroutine(TextAnim(cur_data.text));
    }

    IEnumerator TextAnim(string text) {
        foreach (var i in text) {
            yield return new WaitForSeconds(speed);
            FindObjectOfType<AudioBinder>().PlaySFXOneShot("Intro_Typing");
            msg.text += i;
        }
        yield return null;
    }
}

