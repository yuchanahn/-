using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.SceneManagement;

public class SavePlayerDataForScene : MonoBehaviour
{
    private PlayerInventory inv;
    private Health pc_hp;
    private Stamina pc_sp;
    private SaveLoad sl;
    private Transform pc_pos;
    
    private void Awake()
    {
        sl = FindObjectOfType<SaveLoad>();
        if (sl is null)
        {
            Debug.LogWarning("Scene에 SaveLoad가 없습니다!");
        }
        var pc = FindObjectOfType<PC>();
        pc_pos = pc.transform;
        pc_hp = pc.GetComponent<Health>();
        pc_sp = pc.GetComponent<Stamina>();
        inv = pc.GetComponent<PlayerInventory>();
    }

    private void Start()
    {
        var d = sl.Load<save_ingame_data_t>();
        if (!d.HasValue)
        {
            return;
        }
        if (d.Value.IsNewGame) return;
        pc_hp.set_hp(d.Value.hp);
        pc_sp.value = d.Value.sp;
        inv.potion_cnt = d.Value.potion_count;
    }

    public void save_scene(string scene_name)
    {
        sl.Save(new save_ingame_data_t
        {
            potion_count = inv.potion_cnt,
            hp = (int)pc_hp.value,
            sp = (int)pc_sp.value,
            scene_name = scene_name,
            scene_change = true
        });
    }
    
    public void ResetSaveData()
    {
        sl.Save(new save_ingame_data_t
        {
            reset = true
        });
    }
}
