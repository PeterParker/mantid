# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ui/sans/hfir_instrument.ui'
#
# Created: Fri Apr  1 10:24:47 2011
#      by: PyQt4 UI code generator 4.7.4
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_Frame(object):
    def setupUi(self, Frame):
        Frame.setObjectName("Frame")
        Frame.resize(980, 816)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(Frame.sizePolicy().hasHeightForWidth())
        Frame.setSizePolicy(sizePolicy)
        Frame.setFrameShape(QtGui.QFrame.StyledPanel)
        Frame.setFrameShadow(QtGui.QFrame.Raised)
        self.verticalLayout = QtGui.QVBoxLayout(Frame)
        self.verticalLayout.setObjectName("verticalLayout")
        self.scrollArea = QtGui.QScrollArea(Frame)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.scrollArea.sizePolicy().hasHeightForWidth())
        self.scrollArea.setSizePolicy(sizePolicy)
        self.scrollArea.setMinimumSize(QtCore.QSize(0, 0))
        self.scrollArea.setFrameShape(QtGui.QFrame.NoFrame)
        self.scrollArea.setLineWidth(0)
        self.scrollArea.setWidgetResizable(True)
        self.scrollArea.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.scrollArea.setObjectName("scrollArea")
        self.scrollAreaWidgetContents = QtGui.QWidget(self.scrollArea)
        self.scrollAreaWidgetContents.setGeometry(QtCore.QRect(0, 0, 960, 796))
        self.scrollAreaWidgetContents.setObjectName("scrollAreaWidgetContents")
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.scrollAreaWidgetContents)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.instr_name_label = QtGui.QLabel(self.scrollAreaWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.instr_name_label.sizePolicy().hasHeightForWidth())
        self.instr_name_label.setSizePolicy(sizePolicy)
        self.instr_name_label.setMinimumSize(QtCore.QSize(100, 30))
        self.instr_name_label.setMaximumSize(QtCore.QSize(100, 30))
        font = QtGui.QFont()
        font.setWeight(75)
        font.setBold(True)
        self.instr_name_label.setFont(font)
        self.instr_name_label.setObjectName("instr_name_label")
        self.verticalLayout_4.addWidget(self.instr_name_label)
        self.reduction_options_group = QtGui.QGroupBox(self.scrollAreaWidgetContents)
        self.reduction_options_group.setObjectName("reduction_options_group")
        self.verticalLayout_6 = QtGui.QVBoxLayout(self.reduction_options_group)
        self.verticalLayout_6.setObjectName("verticalLayout_6")
        self.horizontalLayout_5 = QtGui.QHBoxLayout()
        self.horizontalLayout_5.setObjectName("horizontalLayout_5")
        self.label_4 = QtGui.QLabel(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_4.sizePolicy().hasHeightForWidth())
        self.label_4.setSizePolicy(sizePolicy)
        self.label_4.setMinimumSize(QtCore.QSize(150, 0))
        self.label_4.setObjectName("label_4")
        self.horizontalLayout_5.addWidget(self.label_4)
        self.normalization_none_radio = QtGui.QRadioButton(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.normalization_none_radio.sizePolicy().hasHeightForWidth())
        self.normalization_none_radio.setSizePolicy(sizePolicy)
        self.normalization_none_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.normalization_none_radio.setMaximumSize(QtCore.QSize(100, 16777215))
        self.normalization_none_radio.setObjectName("normalization_none_radio")
        self.horizontalLayout_5.addWidget(self.normalization_none_radio)
        self.normalization_time_radio = QtGui.QRadioButton(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.normalization_time_radio.sizePolicy().hasHeightForWidth())
        self.normalization_time_radio.setSizePolicy(sizePolicy)
        self.normalization_time_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.normalization_time_radio.setMaximumSize(QtCore.QSize(100, 16777215))
        self.normalization_time_radio.setObjectName("normalization_time_radio")
        self.horizontalLayout_5.addWidget(self.normalization_time_radio)
        self.normalization_monitor_radio = QtGui.QRadioButton(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.normalization_monitor_radio.sizePolicy().hasHeightForWidth())
        self.normalization_monitor_radio.setSizePolicy(sizePolicy)
        self.normalization_monitor_radio.setMinimumSize(QtCore.QSize(100, 0))
        self.normalization_monitor_radio.setMaximumSize(QtCore.QSize(100, 16777215))
        self.normalization_monitor_radio.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.normalization_monitor_radio.setObjectName("normalization_monitor_radio")
        self.horizontalLayout_5.addWidget(self.normalization_monitor_radio)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_5.addItem(spacerItem)
        self.verticalLayout_6.addLayout(self.horizontalLayout_5)
        self.horizontalLayout_4 = QtGui.QHBoxLayout()
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.label_5 = QtGui.QLabel(self.reduction_options_group)
        self.label_5.setMinimumSize(QtCore.QSize(150, 0))
        self.label_5.setObjectName("label_5")
        self.horizontalLayout_4.addWidget(self.label_5)
        self.scale_edit = QtGui.QLineEdit(self.reduction_options_group)
        self.scale_edit.setMinimumSize(QtCore.QSize(80, 27))
        self.scale_edit.setMaximumSize(QtCore.QSize(80, 27))
        self.scale_edit.setObjectName("scale_edit")
        self.horizontalLayout_4.addWidget(self.scale_edit)
        spacerItem1 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_4.addItem(spacerItem1)
        self.verticalLayout_6.addLayout(self.horizontalLayout_4)
        self.horizontalLayout_7 = QtGui.QHBoxLayout()
        self.horizontalLayout_7.setObjectName("horizontalLayout_7")
        self.dark_file_label = QtGui.QLabel(self.reduction_options_group)
        self.dark_file_label.setMinimumSize(QtCore.QSize(150, 0))
        self.dark_file_label.setMaximumSize(QtCore.QSize(150, 16777215))
        self.dark_file_label.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignVCenter)
        self.dark_file_label.setWordWrap(False)
        self.dark_file_label.setIndent(0)
        self.dark_file_label.setObjectName("dark_file_label")
        self.horizontalLayout_7.addWidget(self.dark_file_label)
        self.dark_file_edit = QtGui.QLineEdit(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.dark_file_edit.sizePolicy().hasHeightForWidth())
        self.dark_file_edit.setSizePolicy(sizePolicy)
        self.dark_file_edit.setMinimumSize(QtCore.QSize(300, 0))
        self.dark_file_edit.setBaseSize(QtCore.QSize(0, 0))
        self.dark_file_edit.setObjectName("dark_file_edit")
        self.horizontalLayout_7.addWidget(self.dark_file_edit)
        self.dark_browse_button = QtGui.QPushButton(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.dark_browse_button.sizePolicy().hasHeightForWidth())
        self.dark_browse_button.setSizePolicy(sizePolicy)
        self.dark_browse_button.setObjectName("dark_browse_button")
        self.horizontalLayout_7.addWidget(self.dark_browse_button)
        self.dark_plot_button = QtGui.QPushButton(self.reduction_options_group)
        self.dark_plot_button.setMinimumSize(QtCore.QSize(85, 27))
        self.dark_plot_button.setMaximumSize(QtCore.QSize(85, 27))
        self.dark_plot_button.setObjectName("dark_plot_button")
        self.horizontalLayout_7.addWidget(self.dark_plot_button)
        spacerItem2 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_7.addItem(spacerItem2)
        self.verticalLayout_6.addLayout(self.horizontalLayout_7)
        self.horizontalLayout_8 = QtGui.QHBoxLayout()
        self.horizontalLayout_8.setObjectName("horizontalLayout_8")
        self.solid_angle_chk = QtGui.QCheckBox(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.solid_angle_chk.sizePolicy().hasHeightForWidth())
        self.solid_angle_chk.setSizePolicy(sizePolicy)
        self.solid_angle_chk.setObjectName("solid_angle_chk")
        self.horizontalLayout_8.addWidget(self.solid_angle_chk)
        self.verticalLayout_6.addLayout(self.horizontalLayout_8)
        self.horizontalLayout_9 = QtGui.QHBoxLayout()
        self.horizontalLayout_9.setObjectName("horizontalLayout_9")
        self.label_2 = QtGui.QLabel(self.reduction_options_group)
        self.label_2.setMinimumSize(QtCore.QSize(150, 0))
        self.label_2.setObjectName("label_2")
        self.horizontalLayout_9.addWidget(self.label_2)
        self.label = QtGui.QLabel(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label.sizePolicy().hasHeightForWidth())
        self.label.setSizePolicy(sizePolicy)
        self.label.setMinimumSize(QtCore.QSize(0, 0))
        self.label.setMaximumSize(QtCore.QSize(150, 16777215))
        self.label.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label.setObjectName("label")
        self.horizontalLayout_9.addWidget(self.label)
        self.n_q_bins_edit = QtGui.QLineEdit(self.reduction_options_group)
        self.n_q_bins_edit.setMinimumSize(QtCore.QSize(80, 27))
        self.n_q_bins_edit.setMaximumSize(QtCore.QSize(80, 27))
        self.n_q_bins_edit.setObjectName("n_q_bins_edit")
        self.horizontalLayout_9.addWidget(self.n_q_bins_edit)
        self.label_3 = QtGui.QLabel(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_3.sizePolicy().hasHeightForWidth())
        self.label_3.setSizePolicy(sizePolicy)
        self.label_3.setMinimumSize(QtCore.QSize(0, 0))
        self.label_3.setMaximumSize(QtCore.QSize(150, 16777215))
        self.label_3.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_3.setObjectName("label_3")
        self.horizontalLayout_9.addWidget(self.label_3)
        self.n_sub_pix_edit = QtGui.QLineEdit(self.reduction_options_group)
        self.n_sub_pix_edit.setMinimumSize(QtCore.QSize(80, 27))
        self.n_sub_pix_edit.setMaximumSize(QtCore.QSize(80, 27))
        self.n_sub_pix_edit.setObjectName("n_sub_pix_edit")
        self.horizontalLayout_9.addWidget(self.n_sub_pix_edit)
        self.log_binning_radio = QtGui.QCheckBox(self.reduction_options_group)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.log_binning_radio.sizePolicy().hasHeightForWidth())
        self.log_binning_radio.setSizePolicy(sizePolicy)
        self.log_binning_radio.setMinimumSize(QtCore.QSize(120, 0))
        self.log_binning_radio.setMaximumSize(QtCore.QSize(120, 16777215))
        self.log_binning_radio.setLayoutDirection(QtCore.Qt.LeftToRight)
        self.log_binning_radio.setObjectName("log_binning_radio")
        self.horizontalLayout_9.addWidget(self.log_binning_radio)
        spacerItem3 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_9.addItem(spacerItem3)
        self.verticalLayout_6.addLayout(self.horizontalLayout_9)
        self.verticalLayout_4.addWidget(self.reduction_options_group)
        self.geometry_options_groupbox = QtGui.QGroupBox(self.scrollAreaWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.geometry_options_groupbox.sizePolicy().hasHeightForWidth())
        self.geometry_options_groupbox.setSizePolicy(sizePolicy)
        self.geometry_options_groupbox.setMinimumSize(QtCore.QSize(0, 0))
        self.geometry_options_groupbox.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.geometry_options_groupbox.setAlignment(QtCore.Qt.AlignLeading|QtCore.Qt.AlignLeft|QtCore.Qt.AlignTop)
        self.geometry_options_groupbox.setObjectName("geometry_options_groupbox")
        self.verticalLayout_5 = QtGui.QVBoxLayout(self.geometry_options_groupbox)
        self.verticalLayout_5.setObjectName("verticalLayout_5")
        self.formLayout_3 = QtGui.QFormLayout()
        self.formLayout_3.setSizeConstraint(QtGui.QLayout.SetDefaultConstraint)
        self.formLayout_3.setFieldGrowthPolicy(QtGui.QFormLayout.AllNonFixedFieldsGrow)
        self.formLayout_3.setObjectName("formLayout_3")
        self.detector_offset_chk = QtGui.QCheckBox(self.geometry_options_groupbox)
        self.detector_offset_chk.setWhatsThis("")
        self.detector_offset_chk.setObjectName("detector_offset_chk")
        self.formLayout_3.setWidget(0, QtGui.QFormLayout.LabelRole, self.detector_offset_chk)
        self.detector_offset_edit = QtGui.QLineEdit(self.geometry_options_groupbox)
        self.detector_offset_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.detector_offset_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.detector_offset_edit.setObjectName("detector_offset_edit")
        self.formLayout_3.setWidget(0, QtGui.QFormLayout.FieldRole, self.detector_offset_edit)
        self.sample_dist_chk = QtGui.QCheckBox(self.geometry_options_groupbox)
        self.sample_dist_chk.setObjectName("sample_dist_chk")
        self.formLayout_3.setWidget(1, QtGui.QFormLayout.LabelRole, self.sample_dist_chk)
        self.sample_dist_edit = QtGui.QLineEdit(self.geometry_options_groupbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.sample_dist_edit.sizePolicy().hasHeightForWidth())
        self.sample_dist_edit.setSizePolicy(sizePolicy)
        self.sample_dist_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.sample_dist_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.sample_dist_edit.setObjectName("sample_dist_edit")
        self.formLayout_3.setWidget(1, QtGui.QFormLayout.FieldRole, self.sample_dist_edit)
        self.wavelength_chk = QtGui.QCheckBox(self.geometry_options_groupbox)
        self.wavelength_chk.setObjectName("wavelength_chk")
        self.formLayout_3.setWidget(2, QtGui.QFormLayout.LabelRole, self.wavelength_chk)
        self.wavelength_edit = QtGui.QLineEdit(self.geometry_options_groupbox)
        self.wavelength_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.wavelength_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.wavelength_edit.setObjectName("wavelength_edit")
        self.formLayout_3.setWidget(2, QtGui.QFormLayout.FieldRole, self.wavelength_edit)
        self.label_10 = QtGui.QLabel(self.geometry_options_groupbox)
        self.label_10.setIndent(23)
        self.label_10.setObjectName("label_10")
        self.formLayout_3.setWidget(3, QtGui.QFormLayout.LabelRole, self.label_10)
        self.wavelength_spread_edit = QtGui.QLineEdit(self.geometry_options_groupbox)
        self.wavelength_spread_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.wavelength_spread_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.wavelength_spread_edit.setObjectName("wavelength_spread_edit")
        self.formLayout_3.setWidget(3, QtGui.QFormLayout.FieldRole, self.wavelength_spread_edit)
        self.verticalLayout_5.addLayout(self.formLayout_3)
        self.verticalLayout_4.addWidget(self.geometry_options_groupbox)
        self.groupBox = QtGui.QGroupBox(self.scrollAreaWidgetContents)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.groupBox.sizePolicy().hasHeightForWidth())
        self.groupBox.setSizePolicy(sizePolicy)
        self.groupBox.setObjectName("groupBox")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.groupBox)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.reduction_options_groupbox = QtGui.QGroupBox(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.reduction_options_groupbox.sizePolicy().hasHeightForWidth())
        self.reduction_options_groupbox.setSizePolicy(sizePolicy)
        self.reduction_options_groupbox.setObjectName("reduction_options_groupbox")
        self.verticalLayout_7 = QtGui.QVBoxLayout(self.reduction_options_groupbox)
        self.verticalLayout_7.setObjectName("verticalLayout_7")
        self.horizontalLayout = QtGui.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.label_6 = QtGui.QLabel(self.reduction_options_groupbox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.label_6.sizePolicy().hasHeightForWidth())
        self.label_6.setSizePolicy(sizePolicy)
        self.label_6.setMinimumSize(QtCore.QSize(38, 0))
        self.label_6.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_6.setObjectName("label_6")
        self.horizontalLayout.addWidget(self.label_6)
        self.top_edit = QtGui.QLineEdit(self.reduction_options_groupbox)
        self.top_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.top_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.top_edit.setObjectName("top_edit")
        self.horizontalLayout.addWidget(self.top_edit)
        self.label_7 = QtGui.QLabel(self.reduction_options_groupbox)
        self.label_7.setMinimumSize(QtCore.QSize(55, 0))
        self.label_7.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_7.setObjectName("label_7")
        self.horizontalLayout.addWidget(self.label_7)
        self.bottom_edit = QtGui.QLineEdit(self.reduction_options_groupbox)
        self.bottom_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.bottom_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.bottom_edit.setObjectName("bottom_edit")
        self.horizontalLayout.addWidget(self.bottom_edit)
        self.label_8 = QtGui.QLabel(self.reduction_options_groupbox)
        self.label_8.setMinimumSize(QtCore.QSize(55, 0))
        self.label_8.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_8.setObjectName("label_8")
        self.horizontalLayout.addWidget(self.label_8)
        self.left_edit = QtGui.QLineEdit(self.reduction_options_groupbox)
        self.left_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.left_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.left_edit.setObjectName("left_edit")
        self.horizontalLayout.addWidget(self.left_edit)
        self.label_9 = QtGui.QLabel(self.reduction_options_groupbox)
        self.label_9.setMinimumSize(QtCore.QSize(55, 0))
        self.label_9.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_9.setObjectName("label_9")
        self.horizontalLayout.addWidget(self.label_9)
        self.right_edit = QtGui.QLineEdit(self.reduction_options_groupbox)
        self.right_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.right_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.right_edit.setObjectName("right_edit")
        self.horizontalLayout.addWidget(self.right_edit)
        spacerItem4 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout.addItem(spacerItem4)
        self.verticalLayout_7.addLayout(self.horizontalLayout)
        self.verticalLayout_2.addWidget(self.reduction_options_groupbox)
        self.geometry_options_groupbox_2 = QtGui.QGroupBox(self.groupBox)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.geometry_options_groupbox_2.sizePolicy().hasHeightForWidth())
        self.geometry_options_groupbox_2.setSizePolicy(sizePolicy)
        self.geometry_options_groupbox_2.setObjectName("geometry_options_groupbox_2")
        self.verticalLayout_3 = QtGui.QVBoxLayout(self.geometry_options_groupbox_2)
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.horizontalLayout_3 = QtGui.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.label_11 = QtGui.QLabel(self.geometry_options_groupbox_2)
        self.label_11.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_11.setObjectName("label_11")
        self.horizontalLayout_3.addWidget(self.label_11)
        self.x_min_edit = QtGui.QLineEdit(self.geometry_options_groupbox_2)
        self.x_min_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.x_min_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.x_min_edit.setObjectName("x_min_edit")
        self.horizontalLayout_3.addWidget(self.x_min_edit)
        self.label_12 = QtGui.QLabel(self.geometry_options_groupbox_2)
        self.label_12.setMinimumSize(QtCore.QSize(55, 0))
        self.label_12.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_12.setObjectName("label_12")
        self.horizontalLayout_3.addWidget(self.label_12)
        self.x_max_edit = QtGui.QLineEdit(self.geometry_options_groupbox_2)
        self.x_max_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.x_max_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.x_max_edit.setObjectName("x_max_edit")
        self.horizontalLayout_3.addWidget(self.x_max_edit)
        self.label_13 = QtGui.QLabel(self.geometry_options_groupbox_2)
        self.label_13.setMinimumSize(QtCore.QSize(55, 0))
        self.label_13.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_13.setObjectName("label_13")
        self.horizontalLayout_3.addWidget(self.label_13)
        self.y_min_edit = QtGui.QLineEdit(self.geometry_options_groupbox_2)
        self.y_min_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.y_min_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.y_min_edit.setObjectName("y_min_edit")
        self.horizontalLayout_3.addWidget(self.y_min_edit)
        self.label_14 = QtGui.QLabel(self.geometry_options_groupbox_2)
        self.label_14.setMinimumSize(QtCore.QSize(55, 0))
        self.label_14.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.label_14.setObjectName("label_14")
        self.horizontalLayout_3.addWidget(self.label_14)
        self.y_max_edit = QtGui.QLineEdit(self.geometry_options_groupbox_2)
        self.y_max_edit.setMinimumSize(QtCore.QSize(80, 0))
        self.y_max_edit.setMaximumSize(QtCore.QSize(80, 16777215))
        self.y_max_edit.setObjectName("y_max_edit")
        self.horizontalLayout_3.addWidget(self.y_max_edit)
        spacerItem5 = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_3.addItem(spacerItem5)
        self.add_rectangle_button = QtGui.QPushButton(self.geometry_options_groupbox_2)
        self.add_rectangle_button.setObjectName("add_rectangle_button")
        self.horizontalLayout_3.addWidget(self.add_rectangle_button)
        self.remove_button = QtGui.QPushButton(self.geometry_options_groupbox_2)
        self.remove_button.setObjectName("remove_button")
        self.horizontalLayout_3.addWidget(self.remove_button)
        spacerItem6 = QtGui.QSpacerItem(31, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_3.addItem(spacerItem6)
        self.verticalLayout_3.addLayout(self.horizontalLayout_3)
        self.horizontalLayout_2 = QtGui.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.listWidget = QtGui.QListWidget(self.geometry_options_groupbox_2)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Expanding)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.listWidget.sizePolicy().hasHeightForWidth())
        self.listWidget.setSizePolicy(sizePolicy)
        self.listWidget.setSelectionMode(QtGui.QAbstractItemView.MultiSelection)
        self.listWidget.setObjectName("listWidget")
        self.horizontalLayout_2.addWidget(self.listWidget)
        spacerItem7 = QtGui.QSpacerItem(32, 20, QtGui.QSizePolicy.Fixed, QtGui.QSizePolicy.Minimum)
        self.horizontalLayout_2.addItem(spacerItem7)
        self.verticalLayout_3.addLayout(self.horizontalLayout_2)
        self.verticalLayout_2.addWidget(self.geometry_options_groupbox_2)
        self.verticalLayout_4.addWidget(self.groupBox)
        self.scrollArea.setWidget(self.scrollAreaWidgetContents)
        self.verticalLayout.addWidget(self.scrollArea)

        self.retranslateUi(Frame)
        QtCore.QMetaObject.connectSlotsByName(Frame)

    def retranslateUi(self, Frame):
        Frame.setWindowTitle(QtGui.QApplication.translate("Frame", "Frame", None, QtGui.QApplication.UnicodeUTF8))
        self.instr_name_label.setText(QtGui.QApplication.translate("Frame", "BIOSANS", None, QtGui.QApplication.UnicodeUTF8))
        self.reduction_options_group.setTitle(QtGui.QApplication.translate("Frame", "Reduction Options", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("Frame", "Normalization", None, QtGui.QApplication.UnicodeUTF8))
        self.normalization_none_radio.setToolTip(QtGui.QApplication.translate("Frame", "Select to skip data normalization.", None, QtGui.QApplication.UnicodeUTF8))
        self.normalization_none_radio.setText(QtGui.QApplication.translate("Frame", "None", None, QtGui.QApplication.UnicodeUTF8))
        self.normalization_time_radio.setToolTip(QtGui.QApplication.translate("Frame", "Select to normalize the data to the data collection time.", None, QtGui.QApplication.UnicodeUTF8))
        self.normalization_time_radio.setText(QtGui.QApplication.translate("Frame", "Time", None, QtGui.QApplication.UnicodeUTF8))
        self.normalization_monitor_radio.setToolTip(QtGui.QApplication.translate("Frame", "Select to normalize the data to the beam monitor count.", None, QtGui.QApplication.UnicodeUTF8))
        self.normalization_monitor_radio.setText(QtGui.QApplication.translate("Frame", "Monitor", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("Frame", "Absolute scale factor", None, QtGui.QApplication.UnicodeUTF8))
        self.scale_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a scaling factor to be multiplied to I(Q).", None, QtGui.QApplication.UnicodeUTF8))
        self.dark_file_label.setText(QtGui.QApplication.translate("Frame", "Dark current data file", None, QtGui.QApplication.UnicodeUTF8))
        self.dark_file_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter a valid file path to be used for the dark current data.", None, QtGui.QApplication.UnicodeUTF8))
        self.dark_browse_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to browse.", None, QtGui.QApplication.UnicodeUTF8))
        self.dark_browse_button.setText(QtGui.QApplication.translate("Frame", "Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.dark_plot_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to plot data 2D data.", None, QtGui.QApplication.UnicodeUTF8))
        self.dark_plot_button.setText(QtGui.QApplication.translate("Frame", "Plot", None, QtGui.QApplication.UnicodeUTF8))
        self.solid_angle_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to perform a solid angle correction.", None, QtGui.QApplication.UnicodeUTF8))
        self.solid_angle_chk.setText(QtGui.QApplication.translate("Frame", "Perform solid angle correction", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("Frame", "Radial averaging", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("Frame", "Number of Q bins  ", None, QtGui.QApplication.UnicodeUTF8))
        self.n_q_bins_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the number of Q bins for the output I(Q) distribution.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("Frame", "Number of sub-pixels", None, QtGui.QApplication.UnicodeUTF8))
        self.n_sub_pix_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the number of sub-pixels in each direction of a detector pixel to use for the\n"
"radial averaging. For instance, entering 3 will sub-divide each detector pixel by 3\n"
"in each direction and will create 9 sub-pixels.", None, QtGui.QApplication.UnicodeUTF8))
        self.log_binning_radio.setToolTip(QtGui.QApplication.translate("Frame", "Select to use logarithmic binning for I(Q).", None, QtGui.QApplication.UnicodeUTF8))
        self.log_binning_radio.setText(QtGui.QApplication.translate("Frame", "Log binning", None, QtGui.QApplication.UnicodeUTF8))
        self.geometry_options_groupbox.setTitle(QtGui.QApplication.translate("Frame", "Experiment Parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.detector_offset_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to set the detector distance offset.", None, QtGui.QApplication.UnicodeUTF8))
        self.detector_offset_chk.setText(QtGui.QApplication.translate("Frame", "Detector distance offset [mm]", None, QtGui.QApplication.UnicodeUTF8))
        self.detector_offset_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the detector distance offset in mm.", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_dist_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to force the sample-detector distance.", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_dist_chk.setText(QtGui.QApplication.translate("Frame", "Sample-detector distance [mm]", None, QtGui.QApplication.UnicodeUTF8))
        self.sample_dist_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the value of the sample-to-detector distance in mm.", None, QtGui.QApplication.UnicodeUTF8))
        self.wavelength_chk.setToolTip(QtGui.QApplication.translate("Frame", "Select to force the wavelength to a given value.", None, QtGui.QApplication.UnicodeUTF8))
        self.wavelength_chk.setText(QtGui.QApplication.translate("Frame", "Wavelength [Angstrom]", None, QtGui.QApplication.UnicodeUTF8))
        self.wavelength_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the value of the neutron wavelength.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("Frame", "Wavelength spread [Angstrom]", None, QtGui.QApplication.UnicodeUTF8))
        self.wavelength_spread_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the value of the neutron wavelength spread.", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox.setTitle(QtGui.QApplication.translate("Frame", "Mask", None, QtGui.QApplication.UnicodeUTF8))
        self.reduction_options_groupbox.setTitle(QtGui.QApplication.translate("Frame", "Edges of the Detector", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("Frame", "Top", None, QtGui.QApplication.UnicodeUTF8))
        self.top_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the number of pixels to be masked from the top of the detector", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("Frame", "Bottom", None, QtGui.QApplication.UnicodeUTF8))
        self.bottom_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the number of pixels to be masked from the bottom of the detector", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("Frame", "Left", None, QtGui.QApplication.UnicodeUTF8))
        self.left_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the number of pixels to be masked from the left side of the detector", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("Frame", "Right", None, QtGui.QApplication.UnicodeUTF8))
        self.right_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the number of pixels to be masked from the right side of the detector", None, QtGui.QApplication.UnicodeUTF8))
        self.geometry_options_groupbox_2.setTitle(QtGui.QApplication.translate("Frame", "Rectangular Masks", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("Frame", "X min", None, QtGui.QApplication.UnicodeUTF8))
        self.x_min_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the minimum pixel number in X that defines your rectangular mask.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("Frame", "X max", None, QtGui.QApplication.UnicodeUTF8))
        self.x_max_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the maximum pixel number in X that defines your rectangular mask.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("Frame", "Y min", None, QtGui.QApplication.UnicodeUTF8))
        self.y_min_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the minimum pixel number in Y that defines your rectangular mask.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("Frame", "Y max", None, QtGui.QApplication.UnicodeUTF8))
        self.y_max_edit.setToolTip(QtGui.QApplication.translate("Frame", "Enter the maximum pixel number in Y that defines your rectangular mask.", None, QtGui.QApplication.UnicodeUTF8))
        self.add_rectangle_button.setToolTip(QtGui.QApplication.translate("Frame", "Click to adda rectangular mask.", None, QtGui.QApplication.UnicodeUTF8))
        self.add_rectangle_button.setText(QtGui.QApplication.translate("Frame", "Add", None, QtGui.QApplication.UnicodeUTF8))
        self.remove_button.setToolTip(QtGui.QApplication.translate("Frame", "Remove the selected mask item from the list.", None, QtGui.QApplication.UnicodeUTF8))
        self.remove_button.setText(QtGui.QApplication.translate("Frame", "Removed Selected", None, QtGui.QApplication.UnicodeUTF8))
        self.listWidget.setToolTip(QtGui.QApplication.translate("Frame", "List of applied rectangular masks.", None, QtGui.QApplication.UnicodeUTF8))

