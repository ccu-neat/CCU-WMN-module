using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports; //要使用serial port需要引用它
using System.Threading;
using System.Collections;



namespace WSN_UI
{
    public partial class Form1 : Form
    {
        private String itemName = "";
        public Form1()
        {
            InitializeComponent();
            string[] myPorts = SerialPort.GetPortNames(); //取得所有port的名字的方法
            cbb_serial_list.DataSource = myPorts;   //直接取得所有port的名字
            cbb_serial_list.DropDownStyle = ComboBoxStyle.DropDownList;

            //ListView
            intiDisplyListView();

            //RxSaveFilePath
            string file_path = System.Windows.Forms.Application.StartupPath;
            string dirName = "Rx_" + DateTime.Now.ToString("yyyyMMdd");
            RxSaveFile = System.IO.Path.Combine(file_path, dirName);
            tv_rx_data_file_path.Text = System.IO.Path.Combine(file_path, dirName);

            //EditView
            ed_max_node.ShortcutsEnabled = false;  // 不啟用快速鍵
            ed_node_id.ShortcutsEnabled = false;  // 不啟用快速鍵
            ed_tx_period.ShortcutsEnabled = false;  // 不啟用快速鍵

        }
        private void Form1_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (comport!=null &&　comport.IsOpen)
            {
                comport.Close();
            }
        }

        private void InitializeComponent()
        {
            this.ckb_lowpower_mcu = new System.Windows.Forms.CheckBox();
            this.label10 = new System.Windows.Forms.Label();
            this.btn_create_ini = new System.Windows.Forms.Button();
            this.ckb_lowpower_rf = new System.Windows.Forms.CheckBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cbb_serial_list = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lb_display_data = new System.Windows.Forms.ListBox();
            this.btn_open_serial = new System.Windows.Forms.Button();
            this.btn_close_serial = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.ckb_sd_open = new System.Windows.Forms.CheckBox();
            this.label12 = new System.Windows.Forms.Label();
            this.ckb_lcd_display = new System.Windows.Forms.CheckBox();
            this.label13 = new System.Windows.Forms.Label();
            this.btn_rx_data_write = new System.Windows.Forms.Button();
            this.tv_rx_data_file_path = new System.Windows.Forms.TextBox();
            this.btn_rx_data_path_select = new System.Windows.Forms.Button();
            this.label14 = new System.Windows.Forms.Label();
            this.trv_network_topology = new System.Windows.Forms.TreeView();
            this.label15 = new System.Windows.Forms.Label();
            this.btn_renew_topology = new System.Windows.Forms.Button();
            this.ed_tx_period = new WSN_UI.NumbersOnlyTextBox();
            this.ed_max_node = new WSN_UI.NumbersOnlyTextBox();
            this.ed_node_id = new WSN_UI.NumbersOnlyTextBox();
            this.btn_list_clear = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // ckb_lowpower_mcu
            // 
            this.ckb_lowpower_mcu.AutoSize = true;
            this.ckb_lowpower_mcu.Location = new System.Drawing.Point(934, 202);
            this.ckb_lowpower_mcu.Name = "ckb_lowpower_mcu";
            this.ckb_lowpower_mcu.Size = new System.Drawing.Size(50, 17);
            this.ckb_lowpower_mcu.TabIndex = 37;
            this.ckb_lowpower_mcu.Text = "啟用";
            this.ckb_lowpower_mcu.UseVisualStyleBackColor = true;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(931, 184);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(67, 13);
            this.label10.TabIndex = 36;
            this.label10.Text = "MCU低功耗";
            // 
            // btn_create_ini
            // 
            this.btn_create_ini.Location = new System.Drawing.Point(956, 234);
            this.btn_create_ini.Name = "btn_create_ini";
            this.btn_create_ini.Size = new System.Drawing.Size(75, 23);
            this.btn_create_ini.TabIndex = 35;
            this.btn_create_ini.Text = "產生設定檔";
            this.btn_create_ini.UseVisualStyleBackColor = true;
            this.btn_create_ini.Click += new System.EventHandler(this.button1_Click);
            // 
            // ckb_lowpower_rf
            // 
            this.ckb_lowpower_rf.AutoSize = true;
            this.ckb_lowpower_rf.Location = new System.Drawing.Point(934, 151);
            this.ckb_lowpower_rf.Name = "ckb_lowpower_rf";
            this.ckb_lowpower_rf.Size = new System.Drawing.Size(50, 17);
            this.ckb_lowpower_rf.TabIndex = 34;
            this.ckb_lowpower_rf.Text = "啟用";
            this.ckb_lowpower_rf.UseVisualStyleBackColor = true;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(931, 135);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(57, 13);
            this.label9.TabIndex = 33;
            this.label9.Text = "RF低功耗";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(775, 133);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(95, 13);
            this.label7.TabIndex = 29;
            this.label7.Text = "資料回傳周期(*2)";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(775, 81);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(55, 13);
            this.label6.TabIndex = 27;
            this.label6.Text = "總節點數";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(775, 8);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(55, 13);
            this.label5.TabIndex = 26;
            this.label5.Text = "燒入參數";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(775, 30);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 13);
            this.label4.TabIndex = 24;
            this.label4.Text = "節點編號(*1)";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(775, 49);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(0, 13);
            this.label3.TabIndex = 23;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(18, 30);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(64, 13);
            this.label2.TabIndex = 22;
            this.label2.Text = "COM PORT";
            // 
            // cbb_serial_list
            // 
            this.cbb_serial_list.FormattingEnabled = true;
            this.cbb_serial_list.Location = new System.Drawing.Point(18, 50);
            this.cbb_serial_list.Name = "cbb_serial_list";
            this.cbb_serial_list.Size = new System.Drawing.Size(159, 21);
            this.cbb_serial_list.TabIndex = 21;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 156);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 20;
            this.label1.Text = "回傳資料";
            // 
            // lb_display_data
            // 
            this.lb_display_data.FormattingEnabled = true;
            this.lb_display_data.Location = new System.Drawing.Point(18, 184);
            this.lb_display_data.Name = "lb_display_data";
            this.lb_display_data.Size = new System.Drawing.Size(718, 342);
            this.lb_display_data.TabIndex = 38;
            // 
            // btn_open_serial
            // 
            this.btn_open_serial.Location = new System.Drawing.Point(21, 87);
            this.btn_open_serial.Name = "btn_open_serial";
            this.btn_open_serial.Size = new System.Drawing.Size(75, 23);
            this.btn_open_serial.TabIndex = 39;
            this.btn_open_serial.Text = "開始";
            this.btn_open_serial.UseVisualStyleBackColor = true;
            this.btn_open_serial.Click += new System.EventHandler(this.btn_open_serial_Click);
            // 
            // btn_close_serial
            // 
            this.btn_close_serial.Location = new System.Drawing.Point(102, 87);
            this.btn_close_serial.Name = "btn_close_serial";
            this.btn_close_serial.Size = new System.Drawing.Size(75, 23);
            this.btn_close_serial.TabIndex = 41;
            this.btn_close_serial.Text = "停止";
            this.btn_close_serial.UseVisualStyleBackColor = true;
            this.btn_close_serial.Click += new System.EventHandler(this.btn_close_serial_Click);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(739, 224);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(179, 13);
            this.label11.TabIndex = 42;
            this.label11.Text = "(*1)無輸入自動產生全部節點資料";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(931, 32);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(58, 13);
            this.label8.TabIndex = 43;
            this.label8.Text = "SD卡紀錄";
            // 
            // ckb_sd_open
            // 
            this.ckb_sd_open.AutoSize = true;
            this.ckb_sd_open.Location = new System.Drawing.Point(934, 50);
            this.ckb_sd_open.Name = "ckb_sd_open";
            this.ckb_sd_open.Size = new System.Drawing.Size(50, 17);
            this.ckb_sd_open.TabIndex = 44;
            this.ckb_sd_open.Text = "啟用";
            this.ckb_sd_open.UseVisualStyleBackColor = true;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(931, 82);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(52, 13);
            this.label12.TabIndex = 45;
            this.label12.Text = "LCD顯示";
            // 
            // ckb_lcd_display
            // 
            this.ckb_lcd_display.AutoSize = true;
            this.ckb_lcd_display.Location = new System.Drawing.Point(934, 101);
            this.ckb_lcd_display.Name = "ckb_lcd_display";
            this.ckb_lcd_display.Size = new System.Drawing.Size(50, 17);
            this.ckb_lcd_display.TabIndex = 46;
            this.ckb_lcd_display.Text = "啟用";
            this.ckb_lcd_display.UseVisualStyleBackColor = true;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(739, 246);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(170, 13);
            this.label13.TabIndex = 47;
            this.label13.Text = "(*2)周期: 輸入的值 * 總節點(sec)";
            // 
            // btn_rx_data_write
            // 
            this.btn_rx_data_write.Location = new System.Drawing.Point(321, 85);
            this.btn_rx_data_write.Name = "btn_rx_data_write";
            this.btn_rx_data_write.Size = new System.Drawing.Size(75, 23);
            this.btn_rx_data_write.TabIndex = 48;
            this.btn_rx_data_write.Text = "開始儲存";
            this.btn_rx_data_write.UseVisualStyleBackColor = true;
            this.btn_rx_data_write.Click += new System.EventHandler(this.btn_rx_data_write_Click);
            // 
            // tv_rx_data_file_path
            // 
            this.tv_rx_data_file_path.Location = new System.Drawing.Point(240, 51);
            this.tv_rx_data_file_path.Name = "tv_rx_data_file_path";
            this.tv_rx_data_file_path.Size = new System.Drawing.Size(181, 20);
            this.tv_rx_data_file_path.TabIndex = 49;
            // 
            // btn_rx_data_path_select
            // 
            this.btn_rx_data_path_select.Location = new System.Drawing.Point(240, 85);
            this.btn_rx_data_path_select.Name = "btn_rx_data_path_select";
            this.btn_rx_data_path_select.Size = new System.Drawing.Size(75, 23);
            this.btn_rx_data_path_select.TabIndex = 50;
            this.btn_rx_data_path_select.Text = "路徑選擇";
            this.btn_rx_data_path_select.UseVisualStyleBackColor = true;
            this.btn_rx_data_path_select.Click += new System.EventHandler(this.btn_rx_data_path_select_Click);
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(237, 30);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(79, 13);
            this.label14.TabIndex = 51;
            this.label14.Text = "資料儲存選項";
            // 
            // trv_network_topology
            // 
            this.trv_network_topology.Location = new System.Drawing.Point(475, 35);
            this.trv_network_topology.Name = "trv_network_topology";
            this.trv_network_topology.Size = new System.Drawing.Size(258, 134);
            this.trv_network_topology.TabIndex = 52;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(475, 8);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(55, 13);
            this.label15.TabIndex = 53;
            this.label15.Text = "網路拓樸";
            // 
            // btn_renew_topology
            // 
            this.btn_renew_topology.Location = new System.Drawing.Point(658, 7);
            this.btn_renew_topology.Name = "btn_renew_topology";
            this.btn_renew_topology.Size = new System.Drawing.Size(75, 23);
            this.btn_renew_topology.TabIndex = 54;
            this.btn_renew_topology.Text = "更新拓樸";
            this.btn_renew_topology.UseVisualStyleBackColor = true;
            this.btn_renew_topology.Click += new System.EventHandler(this.btn_renew_topology_Click);
            // 
            // ed_tx_period
            // 
            this.ed_tx_period.Location = new System.Drawing.Point(778, 149);
            this.ed_tx_period.Name = "ed_tx_period";
            this.ed_tx_period.Size = new System.Drawing.Size(100, 20);
            this.ed_tx_period.TabIndex = 30;
            // 
            // ed_max_node
            // 
            this.ed_max_node.Location = new System.Drawing.Point(778, 97);
            this.ed_max_node.Name = "ed_max_node";
            this.ed_max_node.Size = new System.Drawing.Size(100, 20);
            this.ed_max_node.TabIndex = 28;
            // 
            // ed_node_id
            // 
            this.ed_node_id.Location = new System.Drawing.Point(778, 49);
            this.ed_node_id.Name = "ed_node_id";
            this.ed_node_id.Size = new System.Drawing.Size(100, 20);
            this.ed_node_id.TabIndex = 25;
            // 
            // btn_list_clear
            // 
            this.btn_list_clear.Location = new System.Drawing.Point(76, 151);
            this.btn_list_clear.Name = "btn_list_clear";
            this.btn_list_clear.Size = new System.Drawing.Size(75, 23);
            this.btn_list_clear.TabIndex = 55;
            this.btn_list_clear.Text = "清除";
            this.btn_list_clear.UseVisualStyleBackColor = true;
            this.btn_list_clear.Click += new System.EventHandler(this.btn_list_clear_Click);
            // 
            // Form1
            // 
            this.ClientSize = new System.Drawing.Size(1088, 548);
            this.Controls.Add(this.btn_list_clear);
            this.Controls.Add(this.btn_renew_topology);
            this.Controls.Add(this.label15);
            this.Controls.Add(this.trv_network_topology);
            this.Controls.Add(this.label14);
            this.Controls.Add(this.btn_rx_data_path_select);
            this.Controls.Add(this.tv_rx_data_file_path);
            this.Controls.Add(this.btn_rx_data_write);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.ckb_lcd_display);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.ckb_sd_open);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.btn_close_serial);
            this.Controls.Add(this.btn_open_serial);
            this.Controls.Add(this.lb_display_data);
            this.Controls.Add(this.ckb_lowpower_mcu);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.btn_create_ini);
            this.Controls.Add(this.ckb_lowpower_rf);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.ed_tx_period);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.ed_max_node);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.ed_node_id);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.cbb_serial_list);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private CheckBox ckb_lowpower_mcu;
        private Label label10;
        private Button btn_create_ini;
        private CheckBox ckb_lowpower_rf;
        private Label label9;
        private NumbersOnlyTextBox ed_tx_period;
        private Label label7;
        private NumbersOnlyTextBox ed_max_node;
        private Label label6;
        private Label label5;
        private NumbersOnlyTextBox ed_node_id;
        private Label label4;
        private Label label3;
        private Label label2;
        private ComboBox cbb_serial_list;
        private Label label1;

        //========Node Value start==========
        //Network value - 1
        const int NETWORK_MAX_NODE = 21;
        const int NETWORK_MAX_HOP = 5;
        //LEN
        const int PACKET_LEN_MAX = 32;
        const int PACKET_LEN_UNICAST = PACKET_LEN_MAX;
        //Location
        const int LOCATION_TYPE = 1;
        const int LOCATION_SOU  = 2;
        const int LOCATION_DEST = 3;
        const int LOCATION_HOP = 4;
        const int LOCATION_SYNC = 5;
        const int LOCATION_LEN = 9;
        const int LOCATION_DATA = 10;
        const int LOCATION_UNICAST_DATA = (LOCATION_DATA + NETWORK_MAX_HOP);

        //========Node Value end==========

        //========UI Value start==========
        private SerialPort comport;
        delegate void Display(Byte[] buffer);
        private Int32 totalLength = 0;
        private Byte[] RxData = new Byte[33];
        private int RxLocation = 0;
        private ArrayList DisplayList = new ArrayList();
        private int ErrorCounter = 0;
        private int ErrorCountlimit = 10;//判斷com port是否開對的門檻
        
        //Rx Data save
        private Boolean flag_RxDataSave = false;//是否寫入RX資料
        private string RxSaveFile ="";

        private UInt16[] networkLink = new UInt16[NETWORK_MAX_NODE];

        private UInt16[] FirstSN = new UInt16[NETWORK_MAX_NODE];
        private UInt16[] ReceivePacket = new UInt16[NETWORK_MAX_NODE];
        //========UI Value start==========

        private void comport_DataReceived(Object sender, SerialDataReceivedEventArgs e)
        {
            try
            {
                Byte[] buffer = new Byte[1024];
                Int32 length = (sender as SerialPort).Read(buffer, 0, buffer.Length);
                Array.Resize(ref buffer, length);
                Display d = new Display(DisplayText);
                this.Invoke(d, new Object[] { buffer });
            }
            catch (TimeoutException timeoutEx)
            {
                //以下這邊請自行撰寫你想要的例外處理
                MessageBox.Show(String.Format("出問題啦:{0}", timeoutEx.ToString()));
            }
            catch (Exception ex)
            {
                //以下這邊請自行撰寫你想要的例外處理
                MessageBox.Show(String.Format("出問題啦:{0}", ex.ToString()));
            }
        }
        Boolean FlagPacketStart = false;
        private void DisplayText(Byte[] buffer)
        {
            //TextBox1.Text += String.Format("{0}{1}", BitConverter.ToString(buffer), Environment.NewLine);
            totalLength = totalLength + buffer.Length;

            int LoadLocation = 0;
            while (buffer.Length - LoadLocation > 0)
            {

                if (FlagPacketStart == true)
                {
                    RxData[RxLocation++] = buffer[LoadLocation];
                }else if ((RxLocation == 0 && buffer[LoadLocation].Equals(0xEF) )||FlagPacketStart)
                {
                    FlagPacketStart = true;
                }

                if (RxLocation == PACKET_LEN_UNICAST+1)
                {
                    FlagPacketStart = false;
                    if (RxData[RxLocation - 1].Equals(0xFE))
                    {
                        String result = processRxData();
                        if (flag_RxDataSave) saveRxData(result);
                    }
                    else
                        RxLocation = 0;
                }
                LoadLocation++;
            }


        }

        private void saveRxData(String result)
        {
            if (!result.Equals(""))
            {
                //產生檔案
                string fileName = System.IO.Path.Combine(RxSaveFile, Convert.ToString(DateTime.Now.ToString("yyyyMMdd") +"_"+ (UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)) + ".txt");
                using (System.IO.StreamWriter file = new System.IO.StreamWriter(fileName, true))
                {
                    file.WriteLine(result);
                }

            }
        }


        private String processRxData()
        {
            RxLocation = 0;
            if (RxData[PACKET_LEN_UNICAST - 1] == Checksum(RxData, PACKET_LEN_UNICAST))
            {
                String NodeStatus;
                String NodeID = Convert.ToString((UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff));
                String DataCounter = Convert.ToString((UInt16)(RxData[LOCATION_UNICAST_DATA + 4]) << 8 | (UInt16)(RxData[LOCATION_UNICAST_DATA + 3]));
                String SoilMoisture = Convert.ToString((UInt16)(RxData[LOCATION_UNICAST_DATA + 5] & 0x00ff));
                String Raingague = Convert.ToString((UInt16)(RxData[LOCATION_UNICAST_DATA + 7]) << 8 | (UInt16)(RxData[LOCATION_UNICAST_DATA + 6]));
                String Humidty = Convert.ToString((UInt16)(RxData[LOCATION_UNICAST_DATA + 14] & 0x00ff));
                String Temperature = Convert.ToString((UInt16)(RxData[LOCATION_UNICAST_DATA + 15] & 0x00ff));
                String XAis = Convert.ToString((Int16)((UInt16)(RxData[LOCATION_UNICAST_DATA + 9]) << 8 | (UInt16)(RxData[LOCATION_UNICAST_DATA + 8])));
                String YAis = Convert.ToString((Int16)((UInt16)(RxData[LOCATION_UNICAST_DATA + 11]) << 8 | (UInt16)(RxData[LOCATION_UNICAST_DATA + 10])));
                String ZAis = Convert.ToString((Int16)((UInt16)(RxData[LOCATION_UNICAST_DATA + 13]) << 8 | (UInt16)(RxData[LOCATION_UNICAST_DATA + 11])));
                String[] RoutingPath = new String[NETWORK_MAX_HOP];
                for (UInt16 i = 0; i < NETWORK_MAX_HOP; i++)
                {
                    RoutingPath[i] = Convert.ToString((UInt16)(RxData[LOCATION_DATA + i] & 0x00ff));
                    if (RoutingPath[i].Equals(NodeID))
                    {
                        if (i != 0)
                            networkLink[(UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)] = (UInt16)(RxData[LOCATION_DATA + i - 1] & 0x00ff);
                        else
                            networkLink[(UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)] = 255;
                    }

                }

                String Hop="0";
                for (int i = 0; i < NETWORK_MAX_HOP; i++)
                {
                    if (RxData[LOCATION_UNICAST_DATA + 2] == RxData[LOCATION_DATA + i])
                    {
                        Hop = Convert.ToString(i+1);
                        break;
                    }
                }

                //PDR
                UInt16 FSN = FirstSN[(UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)];
                UInt16 SN = (UInt16)((UInt16)(RxData[LOCATION_UNICAST_DATA + 4]) << 8 | (UInt16)(RxData[LOCATION_UNICAST_DATA + 3]));
                ReceivePacket[(UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)]++;
                int PDR  = 100;
                if (FSN == 0)
                    FirstSN[(UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)] = SN;
                else
                    PDR = Convert.ToInt16(ReceivePacket[(UInt16)(RxData[LOCATION_UNICAST_DATA + 2] & 0x00ff)]*100 / (SN - FSN + 1));

                NodeStatus = "" + NodeID + 
                    "\t " + Convert.ToString(PDR) +"%" + 
                    "\t" + DateTime.Now.ToString("yy/MM/dd HH:mm:ss")+
                    "\t  " + DataCounter +
                    "\t " + Hop +
                    "\t " + SoilMoisture +
                    "\t " + Raingague +
                    "\t " + Humidty +
                    "\t " + Temperature +
                    "\t " + XAis +
                    "\t " + YAis +
                    "\t " + ZAis 
                    ;
                    
                //Data renew
                //lb_display_data.Items.Add(NodeStatus);
                DisplayList.RemoveAt(Convert.ToInt16(NodeID));
                DisplayList.Insert(Convert.ToInt16(NodeID), new ComboItem(NodeStatus,Convert.ToInt16(NodeID)));

                //Display renew
                lb_display_data.DataSource = null;
                lb_display_data.DataSource = DisplayList;
                lb_display_data.DisplayMember = "DisplayText";
                lb_display_data.ValueMember = "Value";
                lb_display_data.Refresh();

                ErrorCounter = 0;

                //return data + Path
                NodeStatus = ConvertStringArrayToString(NodeStatus, RoutingPath);

                return NodeStatus;
            }
            else
            {
                ErrorCounter++;
                if (ErrorCounter > ErrorCountlimit)
                {
                    InitForm1();
                    MessageBox.Show(String.Format("請檢查ComPort是否選擇正確"));
                }
                return "";
            }
        }

        static string ConvertStringArrayToString(string source,string[] array)
        {
            //
            // Concatenate all the elements into a StringBuilder.
            //
            StringBuilder builder = new StringBuilder();

            builder.Append('\t');
            builder.Append(source);
            
            foreach (string value in array)
            {
                builder.Append('\t');
                builder.Append(value);
            }
            return builder.ToString();
        }

        byte Checksum(byte[] packet, int len)
        {
            byte result2 = 0x00;
            for (int i = 0; i < len - 1; i++)
            {
                uint a = (uint)result2 & 0x000000ff;
                uint b = (uint)packet[i] & 0x000000ff;
                result2 = (byte)( a + b);
            }
            result2 = (byte)((uint)0x000000FF - (uint)(result2 &0x000000ff));
            return result2;
        }

        private void btn_open_serial_Click(object sender, EventArgs e)
        {
            if (comport != null && comport.IsOpen) CloseComport(comport);
            comport = new SerialPort(cbb_serial_list.Text, 9600, Parity.None, 8, StopBits.One);
            comport.DataReceived += new SerialDataReceivedEventHandler(comport_DataReceived);
            OpenComport(comport);
            cbb_serial_list.Enabled = false;

        }

        private void CloseComport(SerialPort port)
        {
            try
            {
                if ((port != null) && (port.IsOpen))
                {
                    port.Close();
                }
            }
            catch (Exception ex)
            {
                //這邊你可以自訂發生例外的處理程序
                MessageBox.Show(String.Format("出問題啦:{0}", ex.ToString()));
            }
        }

        private Boolean OpenComport(SerialPort port)
        {
            try
            {
                if ((port != null) && (!port.IsOpen))
                {
                    port.Open();
                }
                return true;
            }
            catch (Exception ex)
            {
                //MessageBox.Show(String.Format("open出問題啦:{0}", ex.ToString()));
                MessageBox.Show("ComPort已被占用");
                return false;
            }
        }


        private void btn_close_serial_Click(object sender, EventArgs e)
        {
            InitForm1();
        }
        private void InitForm1()
        {
            //TextBox1.Clear();
            if (comport != null && comport.IsOpen) CloseComport(comport);
            cbb_serial_list.Enabled = true;

            //初始化DisplayList
            intiDisplyListView();

            if (flag_RxDataSave == true)
            {
                stopRxSave();
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            String NodeID = ed_node_id.Text;
            String MaxNode = ed_max_node.Text;
            String TxPeriod = ed_tx_period.Text;
            Boolean SDOpen = ckb_sd_open.Checked;
            Boolean LCDDisplay = ckb_lcd_display.Checked;
            Boolean RFlowpower = ckb_lowpower_rf.Checked;
            Boolean MCUlowpower = ckb_lowpower_mcu.Checked;

            string[] lines = { NodeID, 
                               MaxNode, 
                               TxPeriod, 
                               SDOpen.Equals(true)? "1":"0",
                               LCDDisplay.Equals(true)? "1":"0",
                               RFlowpower.Equals(true)? "1":"0",
                               MCUlowpower.Equals(true)? "1":"0",
                             };

            if (MaxNode.Equals(""))
            {
                MessageBox.Show("總節點不可為空");
            }
            else if (Convert.ToUInt32(MaxNode) > 21)
            {
                MessageBox.Show("最大節點超過限制(限制:21)");
            }
            else if (!NodeID.Equals("") && Convert.ToUInt32(NodeID) > Convert.ToUInt32(MaxNode))
            {
                MessageBox.Show(String.Format("節點編號超過最大值(限制:{0})", MaxNode));
            }
            else if (Convert.ToUInt32(TxPeriod) > 1000)
            {
                MessageBox.Show(String.Format("超過最大待機時間(值限制:{0})\n目前設定最大可待機{1}分,約{1}小時", 1000, 1000 * Convert.ToUInt32(MaxNode) / 60, 1000 * Convert.ToUInt32(MaxNode) / 360));
            }
            else
            {
                FolderBrowserDialog browser = new FolderBrowserDialog();
                browser.ShowDialog();
                if (browser.SelectedPath.Equals(""))
                {
                    MessageBox.Show("請正確選取儲存路徑");
                }
                else
                {
                    if (NodeID.Equals(""))
                    {
                        for (int i = 0; i < Convert.ToUInt32(MaxNode); i++)
                        {
                            lines[0] = Convert.ToString(i);
                            //產生資料夾
                            string pathString = System.IO.Path.Combine(browser.SelectedPath, Convert.ToString(i));
                            System.IO.Directory.CreateDirectory(pathString);
                            //產生檔案
                            string fileName = System.IO.Path.Combine(pathString, "NodeData.txt");
                            System.IO.File.WriteAllLines(fileName, lines);
                        }
                    }
                    else
                    {
                        //產生資料夾 //@"D:\test\NodeData\
                        string pathString = System.IO.Path.Combine(browser.SelectedPath, NodeID);
                        System.IO.Directory.CreateDirectory(pathString);
                        //產生檔案
                        string fileName = System.IO.Path.Combine(pathString, "NodeData.txt");
                        System.IO.File.WriteAllLines(fileName, lines);
                    }

                    MessageBox.Show("檔案產生成功");
                }
            }

        }

        private void btn_rx_data_path_select_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog browser = new FolderBrowserDialog();
            browser.ShowDialog();
            if (!browser.SelectedPath.Equals(""))
            {
                string dirName = "Rx_" + DateTime.Now.ToString("yyyyMMdd");
                RxSaveFile = System.IO.Path.Combine(browser.SelectedPath, dirName);
                tv_rx_data_file_path.Text = System.IO.Path.Combine(browser.SelectedPath, dirName);
            }
        }

        private void btn_rx_data_write_Click(object sender, EventArgs e)
        {
            if (flag_RxDataSave == false)
            {
                startRxSave();
            }
            else
            {
                stopRxSave();
            }
            
        }
        private void startRxSave()
        {
            flag_RxDataSave = true;
            tv_rx_data_file_path.Enabled = false;
            btn_rx_data_write.Text = "停止儲存";
            //產生資料夾
            System.IO.Directory.CreateDirectory(RxSaveFile);
        }
        private void stopRxSave(){
            flag_RxDataSave = false;
            tv_rx_data_file_path.Enabled = true;
            btn_rx_data_write.Text = "開始儲存";
        }

        private void btn_renew_topology_Click(object sender, EventArgs e)
        {
            TreeNode node = new TreeNode();
            node.Text = "Sink:0";
            
            //buffer 1
            UInt16[] IDarray = new UInt16[20];
            UInt16 location = 0;
            ArrayList treeNode = new ArrayList();

            //buffer 2
            UInt16[] IDarray2 = new UInt16[20];
            UInt16 location2 = 0;
            ArrayList treeNode2 = new ArrayList();

            //Hop 1
            for (UInt16 i = 1; i < NETWORK_MAX_NODE; i++)
            {
                if (networkLink[i] == 255)
                {
                    //add tree
                    TreeNode node1 = new TreeNode();
                    node1.Text = Convert.ToString(i);
                    node.Nodes.Add(node1);
                    //value Store
                    IDarray[location++] = i;
                    treeNode.Add(node1);
                }
            }

            for (UInt16 k = 1; k < NETWORK_MAX_HOP;)
            {
                //Create leaf node of hop 2.
                for (UInt16 j = 0; j < location; j++)//select node of 1 hop.
                {
                    for (UInt16 i = 1; i < NETWORK_MAX_NODE; i++)//search node,if its parent is j.
                    {
                        if (IDarray[j] == networkLink[i])
                        {
                            //add tree
                            TreeNode node1 = new TreeNode();
                            node1.Text = Convert.ToString(i);
                            ((TreeNode)treeNode[j]).Nodes.Add(node1);

                            //value Store
                            IDarray2[location2++] = i;
                            treeNode2.Add(node1);
                        };
                    }
                }
                location = 0;
                treeNode.Clear();
                k++;

                //Create leaf node of hop 3.
                for (UInt16 j = 0; j < location2; j++)//select node of 2 hop.
                {
                    for (UInt16 i = 1; i < NETWORK_MAX_NODE; i++)//search node,if its parent is j.
                    {
                        if (IDarray2[j] == networkLink[i])
                        {
                            //add tree
                            TreeNode node1 = new TreeNode();
                            node1.Text = Convert.ToString(i);
                            ((TreeNode)treeNode2[j]).Nodes.Add(node1);

                            //value Store
                            IDarray[location++] = i;
                            treeNode.Add(node1);
                        };
                    }
                }
                location2 = 0;
                treeNode2.Clear();
                k++;
            }
            trv_network_topology.Nodes.Clear();
            trv_network_topology.Nodes.Add(node);
        }


        private void btn_list_clear_Click(object sender, EventArgs e)
        {
            //初始化DisplayList
            intiDisplyListView();

            //Display renew
            lb_display_data.DataSource = null;
            lb_display_data.DataSource = DisplayList;
            lb_display_data.DisplayMember = "DisplayText";
            lb_display_data.ValueMember = "Value";
            lb_display_data.Refresh();
        }

        private void intiDisplyListView(){
            //初始化DisplayList
            DisplayList.Clear();
            //ListView
            String itemName = "NodeID: " + "\t PDR: "+"\tRT: " + "\t\t  SN: " + "\t Hop: " + "\t土壤: " + "\t雨量: " + "\t濕度: " + "\t 溫度: " + "\t三軸X: " + "\t三軸Y: " + "\t 三軸Z: ";
            DisplayList.Add(new ComboItem(itemName, 0));
            for (int i = 1; i < NETWORK_MAX_NODE; i++)
            {
                DisplayList.Add(new ComboItem(Convert.ToString(i) + "\tNo data", i));
            }
            lb_display_data.DataSource = DisplayList;
            lb_display_data.DisplayMember = "DisplayText";
            lb_display_data.ValueMember = "Value";
        }

    }
}
