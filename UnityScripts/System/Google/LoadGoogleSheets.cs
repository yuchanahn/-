using System.Collections;
using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.Networking;
using System.Linq;
using System.Text;

namespace DB{
    using DB_Data = Dictionary<string, Dictionary<string, string>>;
    public static class GameDB
    {
        public static DB_Data Story = null;
        public static Dictionary<string, DB_Data> AllData = new Dictionary<string, DB_Data>();
        public static string GetText_Sheet2Key(string SheetAndKey)
        {
            string sheet = SheetAndKey.Split('@')[0];
            string key = SheetAndKey.Split('@')[1];
            AllData[sheet][key].TryGetValue("Text", out string text);
            return text;
        }
    }
}

namespace System.Google
{
    using DB_Data = Dictionary<string, Dictionary<string, string>>;
    public class LoadGoogleSheets : MonoBehaviour
    {
        const string format_url = "https://docs.google.com/spreadsheets/d/{0}/gviz/tq?tqx=out:csv&sheet={1}";
        const string docid = "19eBbo42EptOs674uqFAqMvvn-d6HKcZBJqADZuPE6MY";
        public void LoadDB(string sheet_name, Action<DB_Data> endEvent)
        {
            StartCoroutine(DataUpdate(sheet_name, endEvent));
        }

        private IEnumerator DataUpdate(string sheet_name, Action<DB_Data> callback) {
            var www = UnityWebRequest.Get(format_url.Replace("{0}", docid).Replace("{1}", sheet_name));
            yield return www.SendWebRequest();
            var data = www.downloadHandler.text;
            var obj = CsvParser.Parse(data);

            var row = obj.rows;
            var columnNames = row[0].Where(x => !string.IsNullOrEmpty(x)).ToArray();
            
            var dic = new DB_Data();
            foreach (var t in row.Skip(1))
            {
                if(t.Count == 0) continue;
                if(!dic.ContainsKey(t[0].Trim())) dic[t[0].Trim()] = new Dictionary<string, string>();
                for (var i = 0; i < columnNames.Length ; ++i)
                {
                    dic[t[0].Trim()].Add(columnNames[i].Trim(), t[i].Trim());
                }
            }
            
            callback?.Invoke(dic);
        }
    }
}
