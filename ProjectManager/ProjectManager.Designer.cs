namespace ProjectManager
{
    partial class ProjectManager
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProjectManager));
            this.buttonOpenProject = new System.Windows.Forms.Button();
            this.buttonSaveHeader = new System.Windows.Forms.Button();
            this.buttonSaveBinary = new System.Windows.Forms.Button();
            this.buttonPlaySong = new System.Windows.Forms.Button();
            this.buttonExportWav = new System.Windows.Forms.Button();
            this.textBoxOutput = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.buttonProjectDetails = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // buttonOpenProject
            // 
            this.buttonOpenProject.Location = new System.Drawing.Point(12, 12);
            this.buttonOpenProject.Name = "buttonOpenProject";
            this.buttonOpenProject.Size = new System.Drawing.Size(104, 23);
            this.buttonOpenProject.TabIndex = 0;
            this.buttonOpenProject.Text = "Open Project";
            this.buttonOpenProject.UseVisualStyleBackColor = true;
            this.buttonOpenProject.Click += new System.EventHandler(this.buttonOpenProject_Click);
            // 
            // buttonSaveHeader
            // 
            this.buttonSaveHeader.Enabled = false;
            this.buttonSaveHeader.Location = new System.Drawing.Point(12, 138);
            this.buttonSaveHeader.Name = "buttonSaveHeader";
            this.buttonSaveHeader.Size = new System.Drawing.Size(104, 23);
            this.buttonSaveHeader.TabIndex = 0;
            this.buttonSaveHeader.Text = "Save C++ Header";
            this.buttonSaveHeader.UseVisualStyleBackColor = true;
            this.buttonSaveHeader.Click += new System.EventHandler(this.buttonSaveHeader_Click);
            // 
            // buttonSaveBinary
            // 
            this.buttonSaveBinary.Enabled = false;
            this.buttonSaveBinary.Location = new System.Drawing.Point(12, 167);
            this.buttonSaveBinary.Name = "buttonSaveBinary";
            this.buttonSaveBinary.Size = new System.Drawing.Size(104, 23);
            this.buttonSaveBinary.TabIndex = 0;
            this.buttonSaveBinary.Text = "Save Binary Song";
            this.buttonSaveBinary.UseVisualStyleBackColor = true;
            this.buttonSaveBinary.Click += new System.EventHandler(this.buttonSaveBinary_Click);
            // 
            // buttonPlaySong
            // 
            this.buttonPlaySong.Enabled = false;
            this.buttonPlaySong.Location = new System.Drawing.Point(12, 90);
            this.buttonPlaySong.Name = "buttonPlaySong";
            this.buttonPlaySong.Size = new System.Drawing.Size(104, 23);
            this.buttonPlaySong.TabIndex = 0;
            this.buttonPlaySong.Text = "Play Song";
            this.buttonPlaySong.UseVisualStyleBackColor = true;
            this.buttonPlaySong.Click += new System.EventHandler(this.buttonPlaySong_Click);
            // 
            // buttonExportWav
            // 
            this.buttonExportWav.Enabled = false;
            this.buttonExportWav.Location = new System.Drawing.Point(12, 196);
            this.buttonExportWav.Name = "buttonExportWav";
            this.buttonExportWav.Size = new System.Drawing.Size(104, 23);
            this.buttonExportWav.TabIndex = 0;
            this.buttonExportWav.Text = "Export Wav";
            this.buttonExportWav.UseVisualStyleBackColor = true;
            this.buttonExportWav.Click += new System.EventHandler(this.buttonExportWav_Click);
            // 
            // textBoxOutput
            // 
            this.textBoxOutput.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxOutput.Location = new System.Drawing.Point(122, 28);
            this.textBoxOutput.Multiline = true;
            this.textBoxOutput.Name = "textBoxOutput";
            this.textBoxOutput.Size = new System.Drawing.Size(413, 226);
            this.textBoxOutput.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(122, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(108, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Conversion Warnings";
            // 
            // buttonProjectDetails
            // 
            this.buttonProjectDetails.Enabled = false;
            this.buttonProjectDetails.Location = new System.Drawing.Point(11, 41);
            this.buttonProjectDetails.Name = "buttonProjectDetails";
            this.buttonProjectDetails.Size = new System.Drawing.Size(104, 23);
            this.buttonProjectDetails.TabIndex = 4;
            this.buttonProjectDetails.Text = "Project Details";
            this.buttonProjectDetails.UseVisualStyleBackColor = true;
            this.buttonProjectDetails.Click += new System.EventHandler(this.buttonProjectDetails_Click);
            // 
            // ProjectManager
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(547, 266);
            this.Controls.Add(this.buttonProjectDetails);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.textBoxOutput);
            this.Controls.Add(this.buttonExportWav);
            this.Controls.Add(this.buttonPlaySong);
            this.Controls.Add(this.buttonSaveBinary);
            this.Controls.Add(this.buttonSaveHeader);
            this.Controls.Add(this.buttonOpenProject);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ProjectManager";
            this.Text = "Project Manager";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonOpenProject;
        private System.Windows.Forms.Button buttonSaveHeader;
        private System.Windows.Forms.Button buttonSaveBinary;
        private System.Windows.Forms.Button buttonPlaySong;
        private System.Windows.Forms.Button buttonExportWav;
        private System.Windows.Forms.TextBox textBoxOutput;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonProjectDetails;
    }
}

