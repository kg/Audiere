#ifndef WX_AUDIERE_H__INCLUDED_
#define WX_AUDIERE_H__INCLUDED_

#include <wx/wx.h>
#include "audiere.h"

class wxAudiereDeviceDesc
{
public:
	wxAudiereDeviceDesc(const wxString& strName = wxEmptyString, const wxString& strDescription = wxEmptyString)
		: m_strName(strName)
		, m_strDescription(strDescription)
	{
	}
	void SetName(const std::string& strName)
	{
#if wxUSE_UNICODE
		m_strName = wxString(strName.c_str(), wxConvUTF8);
#else
		m_strName = strName.c_str();
#endif
	}
	void SetDescription(const std::string& strDescription)
	{
#if wxUSE_UNICODE
		m_strDescription = wxString(strDescription.c_str(), wxConvUTF8);
#else
		m_strDescription = strDescription.c_str();
#endif
	}
	const wxString& GetName() const { return m_strName; }
	const wxString& GetDescription() const { return m_strDescription; }
private:
	/// Name of device, i.e. "directsound", "winmm", or "oss"
	wxString m_strName;

	// Textual description of device.
	wxString m_strDescription;
};

typedef std::vector<wxAudiereDeviceDesc> wxAudiereDeviceDescArray;

class wxAudiere
{
public:

	static void GetSupportedAudioDevices(wxAudiereDeviceDescArray& arrDevices);

	/**
	* Open a new audio device. If name or parameters are not specified,
	* defaults are used. Each platform has its own set of audio devices.
	* Every platform supports the "null" audio device.
	*
	* @param  strName        name of audio device that should be used
	* @param  strParameters  comma delimited list of audio-device parameters;
	*                         for example, "buffer=100,rate=44100"
	*
	* @return  new audio device object if OpenDevice succeeds, and 0 in case
	*          of failure
	*/
	static audiere::AudioDevicePtr OpenDevice(const wxString& strName = wxEmptyString, const wxString& strParameters = wxEmptyString);



	/**
	  * Create a streaming sample source from a sound file.
	*/
	static audiere::SampleSource* OpenSampleSource(const wxString& strFilename, audiere::FileFormat file_format = audiere::FF_AUTODETECT);

	/**
	* Create a streaming sample source from a input stream.
	*/
	static audiere::SampleSource* OpenSampleSource(wxInputStream& stream, audiere::FileFormat file_format = audiere::FF_AUTODETECT);

	/**
	* Try to open a sound buffer using the specified AudioDevice and
	* input stream.  See also audiere::OpenSound.
	*
	* @param device  AudioDevice in which to open the output stream.
	*
	* @param strFilename  name of the file from where the data will be read.
	*
	* @param streaming  If false or unspecified, OpenSound attempts to
	*                   open the entire sound into memory.  Otherwise, it
	*                   streams the sound from the file.
	*
	* @return  new output stream if successful, 0 otherwise
	*/
	static audiere::OutputStream* OpenSound(const audiere::AudioDevicePtr& device, const wxString& strFilename, bool streaming = false, audiere::FileFormat file_format = audiere::FF_AUTODETECT);

	/**
	* Try to open a sound buffer using the specified AudioDevice and
	* input stream.  See also audiere::OpenSound.
	*
	* @param device  AudioDevice in which to open the output stream.
	*
	* @param stream  Input stream from where the data will be read. You must keep this object
	*                until you delete the OutputStream.
	*
	* @param streaming  If false or unspecified, OpenSound attempts to
	*                   open the entire sound into memory.  Otherwise, it
	*                   streams the sound from the file.
	*
	* @return  new output stream if successful, 0 otherwise
	*/
	static audiere::OutputStream* OpenSound(const audiere::AudioDevicePtr& device, wxInputStream& stream, bool streaming = false, audiere::FileFormat file_format = audiere::FF_AUTODETECT);
};

#endif //!WX_AUDIERE_H__INCLUDED_
