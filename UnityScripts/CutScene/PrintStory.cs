using System.Collections;
using TMPro;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.Serialization;

namespace CutScene
{
    public class PrintStory : MonoBehaviour
    {
        [SerializeField] TextMeshProUGUI storyText;
        [SerializeField] float delay = 0.1f;
        public bool isEnded;
        public bool play_typing_sound;
        private IEnumerator story;
        [SerializeField] UnityEvent onEnd;
        public void Print(int key)
        {
            isEnded = false;
            storyText.text = "";
            if (story is not null) StopCoroutine(story);
            
            story = TextAnim(DB.GameDB.Story[key.ToString()]["Text"]);
            StartCoroutine(story);
        }
        
        public void SetText(string text)
        {
            storyText.text = text;
        }
        
        public void PrintInDB(string sheet_key_colum)
        {
            isEnded = false;
            storyText.text = "";
            if (story is not null) StopCoroutine(story);
            story = TextAnim(DB.GameDB.GetText_Sheet2Key(sheet_key_colum));
            
            
            
            StartCoroutine(story);
        }
        
        
        IEnumerator TextAnim(string text) {
            foreach (var i in text) {
                yield return new WaitForSeconds(delay);
                storyText.text += i;
                if (play_typing_sound)
                {
                    FindObjectOfType<AudioBinder>().PlaySFXOneShot("Intro_Typing");
                }
            }

            isEnded = true;
            onEnd?.Invoke();
            yield return null;
        }
    }
}
