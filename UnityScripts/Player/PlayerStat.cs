using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;


[Serializable] public struct stat_t {
    public float atk_spd;
    public float atk;
    public float def;
}


public class PlayerStat : MonoBehaviour
{
    [SerializeField] public stat_t stat_data;
    
}
