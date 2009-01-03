#pragma once

namespace hlae {
namespace calcu {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			RecalcInternal();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  textBoxWidth;
	private: System::Windows::Forms::TextBox^  textBoxHeight;
	private: System::Windows::Forms::TextBox^  textBoxFPS;
	private: System::Windows::Forms::TextBox^  textBoxMins;
	protected: 

	protected: 



	private: System::Windows::Forms::Label^  label1;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  textBoxSecs;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::GroupBox^  groupBox1;
	private: System::Windows::Forms::TextBox^  textBoxResult;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::Label^  label2;




	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBoxWidth = (gcnew System::Windows::Forms::TextBox());
			this->textBoxHeight = (gcnew System::Windows::Forms::TextBox());
			this->textBoxFPS = (gcnew System::Windows::Forms::TextBox());
			this->textBoxMins = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->textBoxSecs = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->textBoxResult = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// textBoxWidth
			// 
			this->textBoxWidth->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxWidth->Location = System::Drawing::Point(94, 11);
			this->textBoxWidth->MaxLength = 5;
			this->textBoxWidth->Name = L"textBoxWidth";
			this->textBoxWidth->Size = System::Drawing::Size(145, 20);
			this->textBoxWidth->TabIndex = 0;
			this->textBoxWidth->Text = L"800";
			this->textBoxWidth->TextChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// textBoxHeight
			// 
			this->textBoxHeight->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxHeight->Location = System::Drawing::Point(94, 37);
			this->textBoxHeight->MaxLength = 5;
			this->textBoxHeight->Name = L"textBoxHeight";
			this->textBoxHeight->Size = System::Drawing::Size(145, 20);
			this->textBoxHeight->TabIndex = 1;
			this->textBoxHeight->Text = L"600";
			this->textBoxHeight->TextChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// textBoxFPS
			// 
			this->textBoxFPS->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxFPS->Location = System::Drawing::Point(94, 63);
			this->textBoxFPS->MaxLength = 5;
			this->textBoxFPS->Name = L"textBoxFPS";
			this->textBoxFPS->Size = System::Drawing::Size(145, 20);
			this->textBoxFPS->TabIndex = 2;
			this->textBoxFPS->Text = L"30";
			this->textBoxFPS->TextChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// textBoxMins
			// 
			this->textBoxMins->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxMins->Location = System::Drawing::Point(94, 89);
			this->textBoxMins->MaxLength = 5;
			this->textBoxMins->Name = L"textBoxMins";
			this->textBoxMins->Size = System::Drawing::Size(55, 20);
			this->textBoxMins->TabIndex = 3;
			this->textBoxMins->Text = L"1";
			this->textBoxMins->TextChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 14);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(38, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Width:";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(12, 66);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(30, 13);
			this->label3->TabIndex = 6;
			this->label3->Text = L"FPS:";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(12, 92);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(43, 13);
			this->label4->TabIndex = 7;
			this->label4->Text = L"Length:";
			// 
			// textBoxSecs
			// 
			this->textBoxSecs->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxSecs->Location = System::Drawing::Point(184, 89);
			this->textBoxSecs->MaxLength = 5;
			this->textBoxSecs->Name = L"textBoxSecs";
			this->textBoxSecs->Size = System::Drawing::Size(55, 20);
			this->textBoxSecs->TabIndex = 8;
			this->textBoxSecs->Text = L"0";
			this->textBoxSecs->TextChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(155, 92);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(23, 13);
			this->label5->TabIndex = 9;
			this->label5->Text = L"min";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(245, 92);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(24, 13);
			this->label6->TabIndex = 10;
			this->label6->Text = L"sec";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->checkBox1);
			this->groupBox1->Controls->Add(this->textBoxResult);
			this->groupBox1->Location = System::Drawing::Point(12, 126);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(251, 69);
			this->groupBox1->TabIndex = 11;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Estimated Disk Usage";
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(6, 48);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(143, 17);
			this->checkBox1->TabIndex = 4;
			this->checkBox1->Text = L"HuffYuv encoded output";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// textBoxResult
			// 
			this->textBoxResult->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxResult->BackColor = System::Drawing::SystemColors::Window;
			this->textBoxResult->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->textBoxResult->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBoxResult->Location = System::Drawing::Point(6, 19);
			this->textBoxResult->Name = L"textBoxResult";
			this->textBoxResult->ReadOnly = true;
			this->textBoxResult->Size = System::Drawing::Size(239, 23);
			this->textBoxResult->TabIndex = 3;
			this->textBoxResult->Text = L"invalid input";
			this->textBoxResult->TextChanged += gcnew System::EventHandler(this, &Form1::Recalc);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 39);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(41, 13);
			this->label2->TabIndex = 12;
			this->label2->Text = L"Height:";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(278, 209);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->textBoxSecs);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBoxMins);
			this->Controls->Add(this->textBoxFPS);
			this->Controls->Add(this->textBoxHeight);
			this->Controls->Add(this->textBoxWidth);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"Form1";
			this->ShowIcon = false;
			this->Text = L"Calc-o-juGGaKNot";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Recalc(System::Object^  sender, System::EventArgs^  e)
			 {
				 RecalcInternal();
			 }
			 System::Void 			RecalcInternal()
			 {
				 bool bOk = true;
				 System::Int64 width, height, fps, mins, secs;

				 if( System::Int64::TryParse(textBoxWidth->Text,width) )
				 {
					 textBoxWidth->BackColor = System::Drawing::Color::LightGreen;
					 textBoxWidth->ForeColor = System::Drawing::Color::Black;
				 } else {
					 bOk = false;
					 textBoxWidth->BackColor = System::Drawing::Color::Orange;
					 textBoxWidth->ForeColor = System::Drawing::Color::Black;
				 }

 				 if( System::Int64::TryParse(textBoxHeight->Text,height) )
				 {
					 textBoxHeight->BackColor = System::Drawing::Color::LightGreen;
					 textBoxHeight->ForeColor = System::Drawing::Color::Black;
				 } else {
					 bOk = false;
					 textBoxHeight->BackColor = System::Drawing::Color::Orange;
					 textBoxHeight->ForeColor = System::Drawing::Color::Black;
				 }

				 if( System::Int64::TryParse(textBoxFPS->Text,fps) )
				 {
					 textBoxFPS->BackColor = System::Drawing::Color::LightGreen;
					 textBoxFPS->ForeColor = System::Drawing::Color::Black;
				 } else {
					 bOk = false;
					 textBoxFPS->BackColor = System::Drawing::Color::Orange;
					 textBoxFPS->ForeColor = System::Drawing::Color::Black;
				 }

				 if( System::Int64::TryParse(textBoxMins->Text,mins) )
				 {
					 textBoxMins->BackColor = System::Drawing::Color::LightGreen;
					 textBoxMins->ForeColor = System::Drawing::Color::Black;
				 } else {
					 bOk = false;
					 textBoxMins->BackColor = System::Drawing::Color::Orange;
					 textBoxMins->ForeColor = System::Drawing::Color::Black;
				 }

				 if( System::Int64::TryParse(textBoxSecs->Text,secs) )
				 {
					 textBoxSecs->BackColor = System::Drawing::Color::LightGreen;
					 textBoxSecs->ForeColor = System::Drawing::Color::Black;
				 } else {
					 bOk = false;
					 textBoxSecs->BackColor = System::Drawing::Color::Orange;
					 textBoxSecs->ForeColor = System::Drawing::Color::Black;
				 }

				 if( bOk )
				 {
					 System::Decimal result;
					 System::String ^sunit = "Byte";

					 result = width*height*fps*(mins*60+secs)*3;

					 if( this->checkBox1->Checked )
						 result = result / System::Decimal( 2.5f );

					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "KiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "MiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "GiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "TiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "PiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "EiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "ZiB";
					 }
					 if( result >= 1024)
					 {
						 result = result / System::Decimal(1024);
						 sunit = "YiB";
					 }

					System::Globalization::CultureInfo ^MyCI = gcnew System::Globalization::CultureInfo("en-US", false);
					System::Globalization::NumberFormatInfo ^nfi = MyCI->NumberFormat;
					nfi->NumberDecimalDigits = 4;


					textBoxResult->Text = String::Format("{0} {1}",result.ToString("N",nfi),sunit);

					 textBoxResult->BackColor = System::Drawing::Color::LightGreen;
					 textBoxResult->ForeColor = System::Drawing::Color::Black;
				 }
				 else
				 {
					 textBoxResult->Text = "invalid input";
					 textBoxResult->BackColor = System::Drawing::Color::Gray;
					 textBoxResult->ForeColor = System::Drawing::Color::Black;
				 }


			 }
};
} // namespace calcu
} // namespace hlae

