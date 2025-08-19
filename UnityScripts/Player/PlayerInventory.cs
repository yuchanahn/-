using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

public class PlayerInventory : MonoBehaviour
{
    public int potion_cnt = 0;
    public int heal;
    TextMeshProUGUI potion_cnt_text;
    
    private void Awake() {
        
        potion_cnt_text = GameObject.Find("potion_count_text").GetComponent<TextMeshProUGUI>();
        var potion_img = GameObject.Find("potion_img").GetComponent<Image>();
        var potion_amt = GameObject.Find("potion").GetComponent<Animation>();

        var sound = FindObjectOfType<AudioBinder>();
        FindObjectOfType<YCInputSystem>().key.get_event_of_name("UsePotion").press_ev = _ => {
            if ((potion_cnt > 0) && (GetComponent<Health>().value != GetComponent<Health>().max_value)) {
                potion_cnt--;
                
                GetComponent<Health>().value += heal;
                GetComponent<Health>().value = Mathf.Min(GetComponent<Health>().value, GetComponent<Health>().max_value);
                sound.PlaySFXOneShot("Pc_Healing_SFX");
                if (potion_cnt == 0) {
                    var c = potion_img.color;
                    c.a = 0.2f;
                    potion_img.color = c;
                } else {
                    var c = potion_img.color;
                    c.a = 1f;
                    potion_img.color = c;
                }
                

            } else {
                if(potion_cnt == 0) potion_amt.Play("potion_empty");
            }
        };
    }

    private void Update() {
        potion_cnt_text.text = potion_cnt.ToString();
    }
}
