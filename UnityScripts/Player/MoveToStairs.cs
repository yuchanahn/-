using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

public class MoveToStairs : MonoBehaviour
{
    [SerializeField] private float time;
    [SerializeField] private float timer;
    [SerializeField] private Transform targetPoint;
    
    private Transform player;
    private BoxCollider2D playerCollider;
    private Rigidbody2D playerRigidbody;
    private Vector2 playerColSize;
    
    bool IsMoveing = false;
    StairsManager stairsManager;
    private void Awake()
    {
        stairsManager = GetComponentInParent<StairsManager>();
        player = FindObjectOfType<PC>().gameObject.transform;
        playerCollider = player.GetComponent<BoxCollider2D>();
        playerRigidbody = player.GetComponent<Rigidbody2D>();
        playerColSize = playerCollider.size;
    }

    private void Update()
    {
        if(timer < 0 || IsMoveing) timer += Time.deltaTime;
        if (!(timer >= time)) return;
        
        timer = -1f;
        IsMoveing = false;
        stairsManager.MoveStairs(targetPoint);
    }

    private void OnCollisionStay2D(Collision2D other)
    {
        if (!other.gameObject.CompareTag("Player")) return;

        var start = transform.position + Vector3.up * (GetComponent<BoxCollider2D>().size.y / 2) + (Vector3)GetComponent<BoxCollider2D>().offset;
        var hit = Physics2D.RaycastAll(start, Vector2.up, 0.1f);
        
        //draw raycast
        Debug.DrawRay(start, Vector2.up * 0.1f, Color.red);
        if(hit.ToList().Any(x => x.collider.CompareTag("Player"))) return;
        
        player = other.transform;
        IsMoveing = player.GetComponent<PC>().in_move_x != 0 && player.GetComponent<PC>().is_grounded;
        if(!IsMoveing) timer = 0;
    }

    private void OnCollisionExit2D(Collision2D other)
    {
        if (other.gameObject.CompareTag("Player"))
        {
            timer = 0;
            IsMoveing = false;
        }
    }
}
