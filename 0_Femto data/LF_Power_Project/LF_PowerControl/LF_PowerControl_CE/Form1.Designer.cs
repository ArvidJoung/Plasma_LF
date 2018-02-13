namespace LF_PowerControl_CE
{
    partial class Form_Main
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose (bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose ();
            }
            base.Dispose (disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다.
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
        /// </summary>
        private void InitializeComponent ()
        {
            this.components = new System.ComponentModel.Container ();
            this.m_btnClear = new System.Windows.Forms.Button ();
            this.label2 = new System.Windows.Forms.Label ();
            this.label1 = new System.Windows.Forms.Label ();
            this.m_serialPort = new System.IO.Ports.SerialPort (this.components);
            this.m_tbTX = new System.Windows.Forms.TextBox ();
            this.m_tbRX = new System.Windows.Forms.TextBox ();
            this.m_btnSend = new System.Windows.Forms.Button ();
            this.m_btnConnect = new System.Windows.Forms.Button ();
            this.m_Timer = new System.Windows.Forms.Timer ();
            this.m_btnAutoTest = new System.Windows.Forms.Button ();
            this.m_btoAutoTestStop = new System.Windows.Forms.Button ();
            this.m_labelCommErrorCount = new System.Windows.Forms.Label ();
            this.m_tbComPort = new System.Windows.Forms.TextBox ();
            this.m_btnAutoTest200W = new System.Windows.Forms.Button ();
            this.m_tbTX1 = new System.Windows.Forms.TextBox ();
            this.button1 = new System.Windows.Forms.Button ();
            this.button2 = new System.Windows.Forms.Button ();
            this.m_btnCmdF = new System.Windows.Forms.Button ();
            this.m_tbCmdF = new System.Windows.Forms.TextBox ();
            this.m_tbCmdW = new System.Windows.Forms.TextBox ();
            this.m_btnCmdW = new System.Windows.Forms.Button ();
            this.m_tbCmdK = new System.Windows.Forms.TextBox ();
            this.m_btnCmdK = new System.Windows.Forms.Button ();
            this.m_tbCmdL = new System.Windows.Forms.TextBox ();
            this.m_btnCmdL = new System.Windows.Forms.Button ();
            this.m_btnCmdB = new System.Windows.Forms.Button ();
            this.m_tbCmdB = new System.Windows.Forms.TextBox ();
            this.m_tbPower = new System.Windows.Forms.TextBox ();
            this.m_btnCmdP = new System.Windows.Forms.Button ();
            this.m_btnCmdT1 = new System.Windows.Forms.Button ();
            this.m_btnCmdT0 = new System.Windows.Forms.Button ();
            this.button3 = new System.Windows.Forms.Button ();
            this.SuspendLayout ();
            // 
            // m_btnClear
            // 
            this.m_btnClear.Location = new System.Drawing.Point (14, 340);
            this.m_btnClear.Name = "m_btnClear";
            this.m_btnClear.Size = new System.Drawing.Size (131, 32);
            this.m_btnClear.TabIndex = 13;
            this.m_btnClear.Text = "Clear";
            this.m_btnClear.Click += new System.EventHandler (this.m_btnClear_Click_1);
            // 
            // label2
            // 
            this.label2.Font = new System.Drawing.Font ("굴림", 9F, System.Drawing.FontStyle.Bold);
            this.label2.Location = new System.Drawing.Point (14, 256);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size (28, 12);
            this.label2.Text = "RX:";
            // 
            // label1
            // 
            this.label1.Font = new System.Drawing.Font ("굴림", 9F, System.Drawing.FontStyle.Bold);
            this.label1.Location = new System.Drawing.Point (14, 82);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size (28, 12);
            this.label1.Text = "TX:";
            // 
            // m_serialPort
            // 
            this.m_serialPort.BaudRate = 38400;
            this.m_serialPort.PortName = "COM2";
            this.m_serialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler (this.m_serialPort_DataReceived_1);
            // 
            // m_tbTX
            // 
            this.m_tbTX.Location = new System.Drawing.Point (14, 97);
            this.m_tbTX.Name = "m_tbTX";
            this.m_tbTX.Size = new System.Drawing.Size (219, 23);
            this.m_tbTX.TabIndex = 12;
            // 
            // m_tbRX
            // 
            this.m_tbRX.Location = new System.Drawing.Point (14, 271);
            this.m_tbRX.Multiline = true;
            this.m_tbRX.Name = "m_tbRX";
            this.m_tbRX.ReadOnly = true;
            this.m_tbRX.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.m_tbRX.Size = new System.Drawing.Size (500, 63);
            this.m_tbRX.TabIndex = 11;
            // 
            // m_btnSend
            // 
            this.m_btnSend.Enabled = false;
            this.m_btnSend.Location = new System.Drawing.Point (14, 156);
            this.m_btnSend.Name = "m_btnSend";
            this.m_btnSend.Size = new System.Drawing.Size (131, 32);
            this.m_btnSend.TabIndex = 10;
            this.m_btnSend.Text = "Send";
            this.m_btnSend.Click += new System.EventHandler (this.m_btnSend_Click_1);
            // 
            // m_btnConnect
            // 
            this.m_btnConnect.Location = new System.Drawing.Point (14, 14);
            this.m_btnConnect.Name = "m_btnConnect";
            this.m_btnConnect.Size = new System.Drawing.Size (131, 32);
            this.m_btnConnect.TabIndex = 9;
            this.m_btnConnect.Text = "Connect";
            this.m_btnConnect.Click += new System.EventHandler (this.m_btnConnect_Click_1);
            // 
            // m_Timer
            // 
            this.m_Timer.Enabled = true;
            this.m_Timer.Interval = 500;
            this.m_Timer.Tick += new System.EventHandler (this.m_Timer_Tick);
            // 
            // m_btnAutoTest
            // 
            this.m_btnAutoTest.Location = new System.Drawing.Point (246, 14);
            this.m_btnAutoTest.Name = "m_btnAutoTest";
            this.m_btnAutoTest.Size = new System.Drawing.Size (131, 32);
            this.m_btnAutoTest.TabIndex = 16;
            this.m_btnAutoTest.Text = "Auto test (100W)";
            this.m_btnAutoTest.Click += new System.EventHandler (this.m_btnAutoTest_Click);
            // 
            // m_btoAutoTestStop
            // 
            this.m_btoAutoTestStop.Location = new System.Drawing.Point (383, 14);
            this.m_btoAutoTestStop.Name = "m_btoAutoTestStop";
            this.m_btoAutoTestStop.Size = new System.Drawing.Size (131, 32);
            this.m_btoAutoTestStop.TabIndex = 17;
            this.m_btoAutoTestStop.Text = "Stop";
            this.m_btoAutoTestStop.Click += new System.EventHandler (this.m_btoAutoTestStop_Click);
            // 
            // m_labelCommErrorCount
            // 
            this.m_labelCommErrorCount.Font = new System.Drawing.Font ("Arial", 9F, System.Drawing.FontStyle.Regular);
            this.m_labelCommErrorCount.Location = new System.Drawing.Point (197, 82);
            this.m_labelCommErrorCount.Name = "m_labelCommErrorCount";
            this.m_labelCommErrorCount.Size = new System.Drawing.Size (36, 12);
            // 
            // m_tbComPort
            // 
            this.m_tbComPort.Location = new System.Drawing.Point (151, 23);
            this.m_tbComPort.Name = "m_tbComPort";
            this.m_tbComPort.Size = new System.Drawing.Size (63, 23);
            this.m_tbComPort.TabIndex = 20;
            this.m_tbComPort.Text = "COM3";
            // 
            // m_btnAutoTest200W
            // 
            this.m_btnAutoTest200W.Location = new System.Drawing.Point (246, 48);
            this.m_btnAutoTest200W.Name = "m_btnAutoTest200W";
            this.m_btnAutoTest200W.Size = new System.Drawing.Size (131, 32);
            this.m_btnAutoTest200W.TabIndex = 24;
            this.m_btnAutoTest200W.Text = "Auto test (200W)";
            this.m_btnAutoTest200W.Click += new System.EventHandler (this.m_btnAutoTest200W_Click);
            // 
            // m_tbTX1
            // 
            this.m_tbTX1.Location = new System.Drawing.Point (14, 126);
            this.m_tbTX1.Name = "m_tbTX1";
            this.m_tbTX1.Size = new System.Drawing.Size (219, 23);
            this.m_tbTX1.TabIndex = 28;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point (317, 97);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size (60, 32);
            this.button1.TabIndex = 32;
            this.button1.Text = "{A0}";
            this.button1.Click += new System.EventHandler (this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point (383, 97);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size (131, 32);
            this.button2.TabIndex = 33;
            this.button2.Text = "{W0} {L0}";
            this.button2.Click += new System.EventHandler (this.button2_Click);
            // 
            // m_btnCmdF
            // 
            this.m_btnCmdF.Location = new System.Drawing.Point (322, 135);
            this.m_btnCmdF.Name = "m_btnCmdF";
            this.m_btnCmdF.Size = new System.Drawing.Size (55, 32);
            this.m_btnCmdF.TabIndex = 34;
            this.m_btnCmdF.Text = "Freq.";
            this.m_btnCmdF.Click += new System.EventHandler (this.m_btnCmdF_Click);
            // 
            // m_tbCmdF
            // 
            this.m_tbCmdF.Location = new System.Drawing.Point (246, 144);
            this.m_tbCmdF.Name = "m_tbCmdF";
            this.m_tbCmdF.Size = new System.Drawing.Size (70, 23);
            this.m_tbCmdF.TabIndex = 36;
            this.m_tbCmdF.Text = "500";
            // 
            // m_tbCmdW
            // 
            this.m_tbCmdW.Location = new System.Drawing.Point (246, 182);
            this.m_tbCmdW.Name = "m_tbCmdW";
            this.m_tbCmdW.Size = new System.Drawing.Size (70, 23);
            this.m_tbCmdW.TabIndex = 38;
            this.m_tbCmdW.Text = "200";
            // 
            // m_btnCmdW
            // 
            this.m_btnCmdW.Location = new System.Drawing.Point (322, 173);
            this.m_btnCmdW.Name = "m_btnCmdW";
            this.m_btnCmdW.Size = new System.Drawing.Size (55, 32);
            this.m_btnCmdW.TabIndex = 37;
            this.m_btnCmdW.Text = "Width";
            this.m_btnCmdW.Click += new System.EventHandler (this.m_btnCmdW_Click);
            // 
            // m_tbCmdK
            // 
            this.m_tbCmdK.Location = new System.Drawing.Point (383, 144);
            this.m_tbCmdK.Name = "m_tbCmdK";
            this.m_tbCmdK.Size = new System.Drawing.Size (70, 23);
            this.m_tbCmdK.TabIndex = 40;
            this.m_tbCmdK.Text = "500";
            // 
            // m_btnCmdK
            // 
            this.m_btnCmdK.Location = new System.Drawing.Point (459, 135);
            this.m_btnCmdK.Name = "m_btnCmdK";
            this.m_btnCmdK.Size = new System.Drawing.Size (55, 32);
            this.m_btnCmdK.TabIndex = 39;
            this.m_btnCmdK.Text = "{K}";
            this.m_btnCmdK.Click += new System.EventHandler (this.m_btnCmdK_Click);
            // 
            // m_tbCmdL
            // 
            this.m_tbCmdL.Location = new System.Drawing.Point (383, 182);
            this.m_tbCmdL.Name = "m_tbCmdL";
            this.m_tbCmdL.Size = new System.Drawing.Size (70, 23);
            this.m_tbCmdL.TabIndex = 42;
            this.m_tbCmdL.Text = "200";
            // 
            // m_btnCmdL
            // 
            this.m_btnCmdL.Location = new System.Drawing.Point (459, 173);
            this.m_btnCmdL.Name = "m_btnCmdL";
            this.m_btnCmdL.Size = new System.Drawing.Size (55, 32);
            this.m_btnCmdL.TabIndex = 41;
            this.m_btnCmdL.Text = "{L}";
            this.m_btnCmdL.Click += new System.EventHandler (this.m_btnCmdL_Click);
            // 
            // m_btnCmdB
            // 
            this.m_btnCmdB.Location = new System.Drawing.Point (322, 213);
            this.m_btnCmdB.Name = "m_btnCmdB";
            this.m_btnCmdB.Size = new System.Drawing.Size (55, 32);
            this.m_btnCmdB.TabIndex = 43;
            this.m_btnCmdB.Text = "{B}";
            this.m_btnCmdB.Click += new System.EventHandler (this.m_btnCmdB_Click);
            // 
            // m_tbCmdB
            // 
            this.m_tbCmdB.Location = new System.Drawing.Point (246, 222);
            this.m_tbCmdB.Name = "m_tbCmdB";
            this.m_tbCmdB.Size = new System.Drawing.Size (70, 23);
            this.m_tbCmdB.TabIndex = 44;
            this.m_tbCmdB.Text = "200";
            // 
            // m_tbPower
            // 
            this.m_tbPower.Location = new System.Drawing.Point (14, 203);
            this.m_tbPower.Name = "m_tbPower";
            this.m_tbPower.Size = new System.Drawing.Size (70, 23);
            this.m_tbPower.TabIndex = 49;
            this.m_tbPower.Text = "100";
            // 
            // m_btnCmdP
            // 
            this.m_btnCmdP.Location = new System.Drawing.Point (90, 194);
            this.m_btnCmdP.Name = "m_btnCmdP";
            this.m_btnCmdP.Size = new System.Drawing.Size (55, 32);
            this.m_btnCmdP.TabIndex = 48;
            this.m_btnCmdP.Text = "Power";
            this.m_btnCmdP.Click += new System.EventHandler (this.m_btnCmdP_Click);
            // 
            // m_btnCmdT1
            // 
            this.m_btnCmdT1.Location = new System.Drawing.Point (163, 156);
            this.m_btnCmdT1.Name = "m_btnCmdT1";
            this.m_btnCmdT1.Size = new System.Drawing.Size (70, 32);
            this.m_btnCmdT1.TabIndex = 50;
            this.m_btnCmdT1.Text = "ON";
            this.m_btnCmdT1.Click += new System.EventHandler (this.m_btnCmdT1_Click);
            // 
            // m_btnCmdT0
            // 
            this.m_btnCmdT0.Location = new System.Drawing.Point (163, 194);
            this.m_btnCmdT0.Name = "m_btnCmdT0";
            this.m_btnCmdT0.Size = new System.Drawing.Size (70, 32);
            this.m_btnCmdT0.TabIndex = 51;
            this.m_btnCmdT0.Text = "OFF";
            this.m_btnCmdT0.Click += new System.EventHandler (this.m_btnCmdT0_Click);
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point (246, 97);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size (60, 32);
            this.button3.TabIndex = 55;
            this.button3.Text = "{A1}";
            this.button3.Click += new System.EventHandler (this.button3_Click);
            // 
            // Form_Main
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF (96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size (529, 380);
            this.Controls.Add (this.button3);
            this.Controls.Add (this.m_btnCmdT0);
            this.Controls.Add (this.m_btnCmdT1);
            this.Controls.Add (this.m_tbPower);
            this.Controls.Add (this.m_btnCmdP);
            this.Controls.Add (this.m_tbCmdB);
            this.Controls.Add (this.m_btnCmdB);
            this.Controls.Add (this.m_tbCmdL);
            this.Controls.Add (this.m_btnCmdL);
            this.Controls.Add (this.m_tbCmdK);
            this.Controls.Add (this.m_btnCmdK);
            this.Controls.Add (this.m_tbCmdW);
            this.Controls.Add (this.m_btnCmdW);
            this.Controls.Add (this.m_tbCmdF);
            this.Controls.Add (this.m_btnCmdF);
            this.Controls.Add (this.button2);
            this.Controls.Add (this.button1);
            this.Controls.Add (this.m_tbTX1);
            this.Controls.Add (this.m_btnAutoTest200W);
            this.Controls.Add (this.m_tbComPort);
            this.Controls.Add (this.m_labelCommErrorCount);
            this.Controls.Add (this.m_btoAutoTestStop);
            this.Controls.Add (this.m_btnAutoTest);
            this.Controls.Add (this.m_btnClear);
            this.Controls.Add (this.label2);
            this.Controls.Add (this.label1);
            this.Controls.Add (this.m_tbTX);
            this.Controls.Add (this.m_tbRX);
            this.Controls.Add (this.m_btnSend);
            this.Controls.Add (this.m_btnConnect);
            this.Name = "Form_Main";
            this.Text = "LF power control";
            this.ResumeLayout (false);

        }

        #endregion

        private System.Windows.Forms.Button m_btnClear;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.IO.Ports.SerialPort m_serialPort;
        private System.Windows.Forms.TextBox m_tbTX;
        private System.Windows.Forms.TextBox m_tbRX;
        private System.Windows.Forms.Button m_btnSend;
        private System.Windows.Forms.Button m_btnConnect;
        private System.Windows.Forms.Timer m_Timer;
        private System.Windows.Forms.Button m_btnAutoTest;
        private System.Windows.Forms.Button m_btoAutoTestStop;
        private System.Windows.Forms.Label m_labelCommErrorCount;
        private System.Windows.Forms.TextBox m_tbComPort;
        private System.Windows.Forms.Button m_btnAutoTest200W;
        private System.Windows.Forms.TextBox m_tbTX1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button m_btnCmdF;
        private System.Windows.Forms.TextBox m_tbCmdF;
        private System.Windows.Forms.TextBox m_tbCmdW;
        private System.Windows.Forms.Button m_btnCmdW;
        private System.Windows.Forms.TextBox m_tbCmdK;
        private System.Windows.Forms.Button m_btnCmdK;
        private System.Windows.Forms.TextBox m_tbCmdL;
        private System.Windows.Forms.Button m_btnCmdL;
        private System.Windows.Forms.Button m_btnCmdB;
        private System.Windows.Forms.TextBox m_tbCmdB;
        private System.Windows.Forms.TextBox m_tbPower;
        private System.Windows.Forms.Button m_btnCmdP;
        private System.Windows.Forms.Button m_btnCmdT1;
        private System.Windows.Forms.Button m_btnCmdT0;
        private System.Windows.Forms.Button button3;
    }
}

