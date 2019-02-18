namespace ProjectManager
{
    partial class ProjectDetails
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
            this.treeViewDetails = new System.Windows.Forms.TreeView();
            this.SuspendLayout();
            // 
            // treeViewDetails
            // 
            this.treeViewDetails.Dock = System.Windows.Forms.DockStyle.Fill;
            this.treeViewDetails.Location = new System.Drawing.Point(0, 0);
            this.treeViewDetails.Name = "treeViewDetails";
            this.treeViewDetails.Size = new System.Drawing.Size(447, 413);
            this.treeViewDetails.TabIndex = 0;
            // 
            // ProjectDetails
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(447, 413);
            this.Controls.Add(this.treeViewDetails);
            this.Name = "ProjectDetails";
            this.ShowIcon = false;
            this.Text = "Project Details";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView treeViewDetails;
    }
}