using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SerialConnection<T>
{
    System.IO.Ports.SerialPort sp;

    public delegate void OnReceiveData(T data);
    public event OnReceiveData ReceiveData;
    
    public SerialConnection(string device, int baudRate)
    {
        try
        {
            sp = new System.IO.Ports.SerialPort(device, baudRate,
                                                System.IO.Ports.Parity.None, 8,
                                                System.IO.Ports.StopBits.One);
            if (sp != null)
            {
                sp.Open();
            }
            else
            {
                Debug.Log("Couldn't open the USB port");
            }
        }
        catch (Exception e)
        {
            Debug.Log(e.Message);
            sp = null;
        }
    }

    // Update is called once per frame
    public void Update()
    {
        if (sp != null)
        {
            T msg = default(T);
            bool recvMsg = false;
            // Keep pulling messages until we get the most up to date one
            while (sp.BytesToRead > 0)
            {
                try
                {
                    var readData = sp.ReadLine();
                    msg = JsonUtility.FromJson<T>(readData);
                    recvMsg = true;
                }
                catch (Exception ex)
                {
                    Debug.Log(ex.Message);
                }
            }
            if (recvMsg)
            {
                ReceiveData(msg);
            }
        }
    }
}
