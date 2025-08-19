using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace InGameItem
{
    public class YCStoryItem : MonoBehaviour
    {
        [SerializeField] List<Sprite> itemImages;
        [SerializeField] private UnityEvent StartEvent;
        [SerializeField] private UnityEvent EndEvent;
        [SerializeField] Image img;
        [SerializeField] private YCInputSystem input;
        public int itemIndex;
        private bool isrun;
        public bool is_runing {
            get => isrun;

            set
            {
                isrun = value;
                if (isrun)
                {
                    input.key.get_event_of_name("Interaction").press_ev = _ =>
                    {
                        Debug.Log("Item Name : " + itemImages[0].name);
                        if(is_start) End();
                        else Run();
                    };
                }
            }
        }
        public bool is_start = false;

        public void Run()
        {
            if (!is_runing) return;
            FindObjectOfType<AudioBinder>().PlayOneShot("Item_Story_PopUp", eAudioType.SFX);
            StartEvent?.Invoke();
            img.gameObject.SetActive(true);
            img.sprite = itemImages[itemIndex];
            is_start = true;
            input.key.get_event_of_name("StoryNext").press_ev = _ => Next();
            input.key.get_event_of_name("StoryEnd").press_ev = _ => End();
        }

        private void Next()
        {
            if (!is_start) return;
            if (itemImages.Count <= itemIndex + 1) End();
            else img.sprite = itemImages[++itemIndex];
        }

        private void End()
        {
            if (!is_start) return;
            is_start = false;
            is_runing = false;
            img.gameObject.SetActive(false);
            EndEvent?.Invoke();
        }
    }
}
