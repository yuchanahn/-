using System.Collections;
using System.Collections.Generic;
using System.Google;
using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.Serialization;

public class LoadDB_Test : MonoBehaviour
{
    [SerializeField] LoadGoogleSheets loadGoogleSheets;
    [SerializeField] string sheet_name;
    [SerializeField] TextMeshProUGUI text;
    public void load_to_text()
    {
        text.text = "load...\n";
        loadGoogleSheets.LoadDB(sheet_name, (datas) =>
        {
            text.text = "";
            int i = 0;
            foreach (var data in datas)
            {
                //foreach (var value in data.Value)
                //{
                //    text.text += $"{  value.Value } | ";
                //}
                
                i++;
                text.text += $"{  datas[i.ToString()]["Text"] }";
                text.text += "\n--------------------------------------\n";
            }
        });
    }
}
