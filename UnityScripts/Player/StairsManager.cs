using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class StairsManager : MonoBehaviour
{
    private IEnumerator move;
    private Transform cur_target_point;
    private Transform player;
    private Vector2 size;
    private Vector2 half_size;
    private void Awake()
    {
        player = FindObjectOfType<PC>().transform;
        size = player.GetComponent<BoxCollider2D>().size;
        half_size = size / 2;
    }

    public void MoveStairs(Transform target_pos)
    {
        if (cur_target_point is null)
        {
            cur_target_point = target_pos;
            cur_target_point.GetComponentInParent<BoxCollider2D>().enabled = false;
            return;
        }
        if ( cur_target_point.position.y >= target_pos.position.y ) return;
        cur_target_point.GetComponentInParent<BoxCollider2D>().enabled = true;
        cur_target_point = target_pos;
        cur_target_point.GetComponentInParent<BoxCollider2D>().enabled = false;
        player.GetComponent<BoxCollider2D>().size = half_size;
        player.GetComponent<PC>().is_moveable = false;
    }

    private void FixedUpdate()
    {
        if (cur_target_point is null) return;
        var position = player.position;
        var vel = cur_target_point.position - position;
        var plus = vel.normalized * Time.fixedDeltaTime * 30;
        position += plus;
        player.position = position;
        if(Vector3.Distance(position, cur_target_point.position) < 0.1f)
        {
            cur_target_point.GetComponentInParent<BoxCollider2D>().enabled = true;
            player.GetComponent<PC>().is_moveable = true;
            cur_target_point = null;
            player.GetComponent<BoxCollider2D>().size = size;
        }
    }
}
