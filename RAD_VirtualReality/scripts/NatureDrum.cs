using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NatureDrum : MonoBehaviour
{
    public char c = '0'; //which bit to set for this drum

    private Material normalMaterial;
    public Material hitMaterial;
    public GameObject hitObject;

    WebConnection script;

    // Find the script and set up the bytes for this drum
    void Start()
    {
        this.script = GameObject.Find("Ground").GetComponent<WebConnection>();
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
            normalMaterial = hitObject.GetComponent<Renderer>().material;
            hitObject.GetComponent<Renderer>().material = hitMaterial;
        }
    }
    private void OnTriggerExit(Collider other)
    {
        if (script.DEBUG)
        {
            hitObject.GetComponent<Renderer>().material = normalMaterial;
        }
    }
}