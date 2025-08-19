using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;




public class UIOpenSetting : MonoBehaviour {
    [SerializeField] GameObject setting;
    [SerializeField] private GameObject popup;
    
    private void Awake() {
        if (FindObjectOfType<YCInputSystem>().key.Any(x => x.name == "UIOpenSetting")) {
            FindObjectOfType<YCInputSystem>().key.get_event_of_name("UIOpenSetting").press_ev = _ => {
                setting.SetActive(!setting.activeSelf);
                popup.SetActive(!setting.activeSelf);
                setting.GetComponentInChildren<UIView>().enabled = setting.activeSelf;
            };
        }
    }
    
    public void OpenSetting() {
        setting.SetActive(true);
        popup.SetActive(false);
        setting.GetComponentInChildren<UIView>().enabled = true;
    }
}
