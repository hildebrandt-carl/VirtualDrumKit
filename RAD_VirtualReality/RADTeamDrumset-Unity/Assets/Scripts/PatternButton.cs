using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PatternButton : MonoBehaviour
{
    public char c = '0'; //which bit to set for this drum

    WebConnection script;
    Transform boxCollider;

    // Find the script and set up the bytes for this drum
    void Start()
    {
        this.script = GameObject.Find("Ground").GetComponent<WebConnection>();
        this.boxCollider = this.transform.GetChild(0);
    }

    // When drum is hit, set bit
    private void OnTriggerEnter(Collider collider)
    {
        if (c != '0')
        {
            script.AddHit(c);
        }
        if (script.DEBUG)
        {
            Debug.Log("OnTriggerEnter!");
            this.transform.position = this.transform.position + new Vector3(0, -0.05f, 0);
            boxCollider.position = this.transform.position + new Vector3(0, 0.05f, 0);
        }
    }
    private void OnTriggerExit(Collider collider)
    {
        if (script.DEBUG)
        {
            this.transform.position = this.transform.position + new Vector3(0, 0.05f, 0);
            boxCollider.position = this.transform.position;
        }
    }
}