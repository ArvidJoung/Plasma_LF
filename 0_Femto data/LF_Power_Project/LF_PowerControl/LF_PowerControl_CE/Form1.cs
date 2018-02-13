using System;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace LF_PowerControl_CE
{
    public partial class Form_Main : Form
    {
        string m_strRX = "";
        bool m_isRxUpdate = false;

        int m_nTimerCount = 0;
        bool m_isAutoTest = false;

        int m_nPower = 100;
        int m_nTestMaxPower = 1000;

        int m_nCommErrorCount = 0;

        public Form_Main ()
        {
            InitializeComponent ();
        }

        private byte CalCheckSum_Base64 (byte[] data, int start, int len)
        {
            int i = 0;
            int ret = 0;

            for (i = 0; i < len; i++)
            {
                ret += data[start + i];
            }

            return (byte)((byte)'0' + (byte)(ret % 64));
        }

        private void Tx_Cmd (byte[] data, int len)
        {
	        byte[] buf = new byte[256];

            Array.Copy (data, buf, len);

	        buf[len] = buf[len - 1];
	        buf[len - 1] = CalCheckSum_Base64 (buf, 1, len - 2);
	        len++;

            m_serialPort.Write (buf, 0, len);
        }


        private void TxString (string str)
        {
            byte[] tx_buf = Encoding.UTF8.GetBytes (str);
            Tx_Cmd (tx_buf, tx_buf.Length);

            bool is_rx_ok = false;
            m_strRX = "";

            int i;
            for (i = 0; i < 20; i++)
            {
                Thread.Sleep (50);

                if (m_strRX.Length > 2 && m_strRX[0] == '{' && m_strRX.EndsWith ("}"))
                {
                    // CheckSum.
                    byte[] rx_buf = Encoding.UTF8.GetBytes (m_strRX);
                    byte check_sum = CalCheckSum_Base64 (rx_buf, 1, rx_buf.Length - 3);

                    if (rx_buf[rx_buf.Length - 2] == check_sum)
                    {
                        rx_buf[rx_buf.Length - 2] = rx_buf[rx_buf.Length - 1];
                        m_strRX = Encoding.Default.GetString (rx_buf, 0, rx_buf.Length - 1);

                        is_rx_ok = true;
                        break;
                    }
                    else
                    {
                        m_strRX += "CheckSum error!!!";
                        break;
                    }

                    //// check CMD.
                    //if (m_strRX[1] != str[1])
                    //{
                    //    if (m_strRX[1] != 'E')
                    //    {
                    //        m_strRX = "CMD error!!!";
                    //    }
                    //    break;
                    //}
                }
            }

            if (i >= 20)
            {
                m_strRX = "Timeout error!!!";
            }

            if (!is_rx_ok)
            {
                m_nCommErrorCount++;
            }
        }

        private void m_btnConnect_Click_1 (object sender, EventArgs e)
        {
            try
            {
                m_serialPort.PortName = m_tbComPort.Text.Trim ();
                m_serialPort.Open ();
            }
            catch (Exception ex)
            {
                MessageBox.Show ("Fail to open!!!");
                return;
            }

            m_btnSend.Enabled = true;
        }

        private void m_btnSend_Click_1 (object sender, EventArgs e)
        {
            if (m_tbTX.Text.Trim().Length > 0)
                TxString (m_tbTX.Text);

            Thread.Sleep (30);

            if (m_tbTX1.Text.Trim().Length > 0)
                TxString (m_tbTX1.Text);

            /*
            string str = m_tbTX.Text;
            byte[] txb = new byte[str.Length];
            char[] txc = m_tbTX.Text.ToCharArray ();

            for (int i = 0; i < str.Length; i++)
            {
                txb[i] = (byte)txc[i];
            }

            m_serialPort.Write (txb, 0, txb.Length);
            */
        }

        private void m_btnClear_Click_1 (object sender, EventArgs e)
        {
            m_strRX = "";
            m_tbRX.Text = "";
        }

        private void m_serialPort_DataReceived_1 (object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            int len = m_serialPort.BytesToRead;

            for (int i = 0; i < len; i++)
            {
                m_strRX += (char)m_serialPort.ReadByte ();
            }

            m_isRxUpdate = true;
        }

        private void m_Timer_Tick (object sender, EventArgs e)
        {
            if (m_isAutoTest)
            {
                string str = "";

                if (m_nTimerCount % 70 == 0)
                {
                    TxString ("{A1}");

                    str = "{P" + m_nPower + "}";

                    m_nPower += 100;
                    if (m_nPower > m_nTestMaxPower) m_nPower = 100;

                    TxString (str);
                    m_tbTX.Text = str;
                }

                if (m_nTimerCount % 70 == 60)
                {
                    str = "{P0}";

                    TxString (str);
                    m_tbTX.Text = str;
                }

                if (m_nTimerCount % 700 == 0)
                {
                    m_tbRX.Text = "";
                }

                m_nTimerCount++;
            }

            if (m_isRxUpdate)
            {
                m_isRxUpdate = false;
                m_tbRX.Text = m_strRX;
            }

            m_labelCommErrorCount.Text = "" + m_nCommErrorCount;
        }

        private void m_btnAutoTest_Click (object sender, EventArgs e)
        {
            m_isAutoTest = true;
            m_nTimerCount = 0;
            m_nCommErrorCount = 0;
            m_nTestMaxPower = 1000;

            m_btnSend.Enabled = false;
        }

        private void m_btoAutoTestStop_Click (object sender, EventArgs e)
        {
            m_isAutoTest = false;
            m_btnSend.Enabled = true;
        }

        private void m_btnAutoTest200W_Click (object sender, EventArgs e)
        {
            m_isAutoTest = true;
            m_nTimerCount = 0;
            m_nCommErrorCount = 0;
            m_nTestMaxPower = 2000;

            m_btnSend.Enabled = false;
        }

        private void button2_Click (object sender, EventArgs e)
        {
            TxString ("{W0}");
            Thread.Sleep (100);
            TxString ("{L0}");
        }

        private void button1_Click (object sender, EventArgs e)
        {
            TxString ("{A0}");
        }

        private void m_btnCmdF_Click (object sender, EventArgs e)
        {
            if (m_tbCmdF.Text.Trim ().Length > 0)
                TxString ("{F" + m_tbCmdF.Text.Trim () + "}");
        }

        private void m_btnCmdW_Click (object sender, EventArgs e)
        {
            if (m_tbCmdW.Text.Trim ().Length > 0)
                TxString ("{W" + m_tbCmdW.Text.Trim () + "}");
        }

        private void m_btnCmdB_Click (object sender, EventArgs e)
        {
            if (m_tbCmdB.Text.Trim ().Length > 0)
                TxString ("{B" + m_tbCmdB.Text.Trim () + "}");
        }

        private void m_btnCmdK_Click (object sender, EventArgs e)
        {
            if (m_tbCmdK.Text.Trim ().Length > 0)
                TxString ("{K" + m_tbCmdK.Text.Trim () + "}");
        }

        private void m_btnCmdL_Click (object sender, EventArgs e)
        {
            if (m_tbCmdL.Text.Trim ().Length > 0)
                TxString ("{L" + m_tbCmdL.Text.Trim () + "}");
        }

        private void m_btnCmdT1_Click (object sender, EventArgs e)
        {
            TxString ("{T1}");
        }

        private void m_btnCmdT0_Click (object sender, EventArgs e)
        {
            TxString ("{T0}");
        }

        private void m_btnCmdP_Click (object sender, EventArgs e)
        {
            if (m_tbPower.Text.Trim ().Length > 0)
                TxString ("{P" + m_tbPower.Text.Trim () + "}");
        }

        private void button3_Click (object sender, EventArgs e)
        {
            TxString ("{A1}");
        }
    }
}