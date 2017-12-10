using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Drum : MonoBehaviour
{
    public char c = '0'; //which bit to set for this drum
    
    private Material normalMaterial;
    public Material hitMaterial;

    WebConnection script;

    // Find the script and set up the bytes for this drum
    void Start()
    {
        this.script = GameObject.Find("Ground").GetComponent<WebConnection>();
        normalMaterial = this.GetComponent<Renderer>().material;
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
            this.GetComponent<Renderer>().material = hitMaterial;
        }
    }
    private void OnTriggerExit(Collider other)
    {
        if (script.DEBUG)
        {
            this.GetComponent<Renderer>().material = normalMaterial;
        }
    }
}