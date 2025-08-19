using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.UI;

public class UIKeyBind : MonoBehaviour
{
    [SerializeField] YCInputSystem inputSystem;
    [SerializeField] private GameObject prefab;
    [SerializeField] private List<GameObject> slots;

    [SerializeField] private RectTransform start;
    [SerializeField] private Vector3 offset;
    private bool is_changing = false;
    private void Awake()
    {
        foreach (var i in inputSystem.key)
        {
            if(i.tag != EInput_tag.player) continue;
            var slot = Instantiate(prefab, start.position + offset * -slots.Count, Quaternion.identity);
            slot.transform.SetParent(transform);
            slot.GetComponentInChildren<TextMeshProUGUI>().text = i.name;
            slot.GetComponentInChildren<Button>().GetComponentInChildren<TextMeshProUGUI>().text = i.key.Last().ToString();
            slots.Add(slot);
            slot.GetComponentInChildren<Button>().onClick.AddListener(() =>
            {
                if (is_changing) return;
                ChangeKey(slot);
            });
        }
    }
    
    public void ChangeKey(GameObject slot)
    {
        var key = slot.GetComponentInChildren<Button>().GetComponentInChildren<TextMeshProUGUI>();
        StartCoroutine(ChangeKeyCoroutine(key));
    }
    
    IEnumerator ChangeKeyCoroutine(TextMeshProUGUI key)
    {
        is_changing = true;
        var keyInput = inputSystem.key.First(i => i.key.Last().ToString() == key.text);
        key.text = "Press a key";
        var c = key.color;
        key.color = Color.red;
        yield return new WaitUntil(() => Input.anyKeyDown);
        foreach (KeyCode code in Enum.GetValues(typeof(KeyCode)))
        {
            if (!Input.GetKeyDown(code)) continue;
            keyInput.key = new KeyCode[] { code };
            key.text = code.ToString();
            key.color = c;
            is_changing = false;

            var sl = FindObjectOfType<SaveLoad>();
            var d = sl.Load<save_system_data_t>();
            var save = d ?? new save_system_data_t();
            if (!d.HasValue) save.key_bind_data = new Dictionary<string, KeyCode[]>();  
            save.key_bind_data[keyInput.name] = keyInput.key; 
            sl.Save(save);
            
            break;
        }
    }
    
}
