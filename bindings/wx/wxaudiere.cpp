#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/defs.h"
#endif
#include "wx/wfstream.h"

#include "wxaudiere.h"

//////////////////////////////////////////////////////////////////////////
// wxAudiereFile
//////////////////////////////////////////////////////////////////////////

class wxAudiereFile: public audiere::RefImplementation<audiere::File>
{
public:
	wxAudiereFile(wxInputStream *pInputStream, bool bDestroy = true)
		: m_pInputStream(pInputStream)
		, m_bDestroyInputStream(bDestroy)
	{
	}
	virtual ~wxAudiereFile()
	{
		if (m_bDestroyInputStream)
			delete m_pInputStream;
		m_pInputStream = NULL;
	}
	int ADR_CALL read(void* buffer, int length)
	{
		if (m_pInputStream == NULL) return 0;
		m_pInputStream->Read(buffer, (size_t)length);
		return m_pInputStream->LastRead();
	}

	bool ADR_CALL seek(int position, SeekMode mode)
	{
		if (m_pInputStream == NULL) return false;
		wxFileOffset fileOffset;
		switch (mode) {
		case END:
			fileOffset = m_pInputStream->SeekI(position, wxFromEnd);
			break;
		case BEGIN:
			fileOffset = m_pInputStream->SeekI(position, wxFromStart);
			break;
		case CURRENT:
			fileOffset = m_pInputStream->SeekI(position, wxFromCurrent);
			break;
		default:
			fileOffset = wxInvalidOffset;
		}
		return fileOffset != wxInvalidOffset;
	}
	int ADR_CALL tell()
	{
		if (m_pInputStream == NULL) return 0;
		return m_pInputStream->TellI();
	}
private:
	wxAudiereFile(const wxAudiereFile& ) {}
	wxAudiereFile& operator=(const wxAudiereFile& ) {}

	wxInputStream *m_pInputStream;
	bool m_bDestroyInputStream;
};

typedef audiere::RefPtr<wxAudiereFile> wxAudiereFilePtr;

//////////////////////////////////////////////////////////////////////////
// wxAudiere class implementation
//////////////////////////////////////////////////////////////////////////

void wxAudiere::GetSupportedAudioDevices(wxAudiereDeviceDescArray& arrDevices)
{
	std::vector<std::string> descriptions;
	audiere::SplitString(descriptions, audiere::hidden::AdrGetSupportedAudioDevices(), ';');

	arrDevices.clear();
	arrDevices.resize(descriptions.size());
	std::vector<std::string> d;
	for (unsigned i = 0; i < descriptions.size(); ++i) {
		d.clear();
		audiere::SplitString(d, descriptions[i].c_str(), ':');
		arrDevices[i].SetName(d[0]);
		arrDevices[i].SetDescription(d[1]);
	}
}

audiere::AudioDevicePtr wxAudiere::OpenDevice(const wxString& strName, const wxString& strParameters)
{
#if wxUSE_UNICODE
	char *szName = NULL, *szParameters = NULL;
	wxCharBuffer charBufferName((const char *)NULL), charBufferParameters((const char *)NULL);
	if (!strName.IsEmpty()) {
		charBufferName = strName.mb_str(wxConvUTF8);
		szName = charBufferName.data();
	}
	if (!strParameters.IsEmpty()) {
		charBufferParameters = strParameters.mb_str(wxConvUTF8);
		szParameters = charBufferParameters.data();
	}
	return audiere::OpenDevice(szName, szParameters);
#else
	return audiere::OpenDevice(strName.GetData(), strParameters.GetData());
#endif
}

audiere::SampleSource* wxAudiere::OpenSampleSource(const wxString& strFilename, audiere::FileFormat file_format)
{
	wxAudiereFilePtr audiereFile(new wxAudiereFile(new wxFileInputStream(strFilename), true));
	if (audiereFile.get() == NULL) return NULL;
	return audiere::OpenSampleSource(audiereFile.get(), file_format);
}

audiere::SampleSource* wxAudiere::OpenSampleSource(wxInputStream& stream, audiere::FileFormat file_format)
{
	wxAudiereFilePtr audiereFile = new wxAudiereFile(&stream, false);
	if (audiereFile.get() == NULL) return NULL;
	return audiere::OpenSampleSource(audiereFile.get(), file_format);
}

audiere::OutputStream* wxAudiere::OpenSound(const audiere::AudioDevicePtr& device, const wxString& strFilename, bool streaming, audiere::FileFormat file_format)
{
	wxAudiereFilePtr audiereFile = new wxAudiereFile(new wxFileInputStream(strFilename), true);
	if (audiereFile.get() == NULL) return NULL;
	return audiere::OpenSound(device, audiereFile.get(), streaming, file_format);
}

audiere::OutputStream* wxAudiere::OpenSound(const audiere::AudioDevicePtr& device, wxInputStream& stream, bool streaming, audiere::FileFormat file_format)
{
	wxAudiereFilePtr audiereFile = new wxAudiereFile(&stream, false);
	if (audiereFile.get() == NULL) return NULL;
	return audiere::OpenSound(device, audiereFile.get(), streaming, file_format);
}
