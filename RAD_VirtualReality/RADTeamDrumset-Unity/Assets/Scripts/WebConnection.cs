using UnityEngine;

using System;
using System.Net.Sockets;
using System.IO;

public class WebConnection : MonoBehaviour {
    String myip = "160.119.248.28";
    int port = 4242;

    TcpClient socket;
    NetworkStream stream;
    StreamWriter writer;
    StreamReader reader;
    bool socketReady;
    
    private String msg;
    private int ackTimeout;

    public bool TESTING = false; // set to avoid actually connecting to web socket
    public bool DEBUG = true; // print/show debug information
    
    // Set up the socket on startup
    void Start()
    {
        socketReady = false;
        connectSocket();
        msg = "";
    }
	
    // Add flag for drum hit
    public void AddHit(char c)
    {
        msg = msg + c;
    }
    // Check if socket is connected
    public bool isConnected()
    {
        return socketReady;
    }
    // (Re)Connect socket
    public void connectSocket()
    {
        if (DEBUG) { Debug.Log("Setting up socket..."); }
        setupSocket();
        writeSocket("vr");
    }

    // Send message after all drums could be hit
    void LateUpdate () {
        // receive an acknowledgement
        String str = readSocket();
        if (DEBUG) { Debug.Log("read from socket:" + str);  }
        if (ackTimeout >=0 && (str != "ack"))
        {
            ackTimeout--; // decrement the timeout counter
            if (ackTimeout == 0)
            {
                socketReady = false; // if timeout counter reaches 0, assume we are disconnected
            }
        }
        else if(str == "ack")
        {
            ackTimeout = -1; // got an ack, disable the timeout counter
            socketReady = true;
        }
        // send data from this frame and reset for next frame
        if (msg!="")
        {
            writeSocket(msg);
            ackTimeout = 30; // sent a message, start the timeout counter
            if (DEBUG) { Debug.Log("wrote to socket: " + msg); }
        }
        msg = "";
    }

    // Close the socket on shutdown
    private void OnDestroy()
    {
        closeSocket();
    }


    // HELPER FUNCTIONS
    private void setupSocket()
    {
        if (TESTING) { return; }
        try
        {
            socket = new TcpClient(myip, port);
            stream = socket.GetStream();
            writer = new StreamWriter(stream);
            reader = new StreamReader(stream);
            socketReady = true;
            Debug.Log("Socket connected.");
        }
        catch (Exception e)
        {
            Debug.Log("SOCKET ERROR: " + e);
        }
    }
    private void writeSocket(string theLine)
    {
        if (TESTING) { return; }
        try
        {
            writer.Write(theLine);
            writer.Flush();
        }
        catch (Exception e)
        {
            socketReady = false;
        }
    }
    private String readSocket()
    {
        if (TESTING) { return ""; }
        try
        { 
            if(stream.DataAvailable)
            {
                return reader.ReadLine();
            }
            return "";
        }
        catch (Exception e)
        {
            socketReady = false;
        }
        return "";
    }

    private void closeSocket()
    {
        if (TESTING) { return; }
        socketReady = false;
        writer.Close();
        reader.Close();
        socket.Close();
    }
}
