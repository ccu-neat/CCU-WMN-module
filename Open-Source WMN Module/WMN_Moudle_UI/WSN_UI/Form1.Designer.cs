namespace WSN_UI
{
    partial class Form1
    {
        /// <summary>
        /// 設計工具所需的變數。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清除任何使用中的資源。
        /// </summary>
        /// <param name="disposing">如果應該處置 Managed 資源則為 true，否則為 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private System.Windows.Forms.ListBox lb_display_data;
        private System.Windows.Forms.Button btn_open_serial;
        private System.Windows.Forms.Button btn_close_serial;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.CheckBox ckb_sd_open;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.CheckBox ckb_lcd_display;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Button btn_rx_data_write;
        private System.Windows.Forms.TextBox tv_rx_data_file_path;
        private System.Windows.Forms.Button btn_rx_data_path_select;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TreeView trv_network_topology;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Button btn_renew_topology;
        private System.Windows.Forms.Button btn_list_clear;

    }
}

