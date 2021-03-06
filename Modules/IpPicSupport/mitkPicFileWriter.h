/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/


#ifndef _MITK_PIC_FILE_WRITER__H_
#define _MITK_PIC_FILE_WRITER__H_

#include <MitkIpPicSupportExports.h>
#include <mitkFileWriter.h>
#include <mitkIpPic.h>

#include "mitkLegacyAdaptors.h"


namespace mitk
{
class Image;
/**
 * @brief Writer for mitk::Image
 * @ingroup IO
 */
class MitkIpPicSupport_EXPORT PicFileWriter : public mitk::FileWriter
{
public:

    mitkClassMacro( PicFileWriter, mitk::FileWriter );

    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    mitkWriterMacro;

    /**
     * Sets the filename of the file to write.
     * @param _arg the name of the file to write.
     */
    itkSetStringMacro( FileName );

    /**
     * @returns the name of the file to be written to disk.
     */
    itkGetStringMacro( FileName );

    /**
     * @warning multiple write not (yet) supported
     */
    itkSetStringMacro( FilePrefix );

    /**
     * @warning multiple write not (yet) supported
     */
    itkGetStringMacro( FilePrefix );

    /**
     * @warning multiple write not (yet) supported
     */
    itkSetStringMacro( FilePattern );

    /**
     * @warning multiple write not (yet) supported
     */
    itkGetStringMacro( FilePattern );

    /**
     * Sets the 0'th input object for the filter.
     * @param input the first input for the filter.
     */
    void SetInputImage( mitk::Image* input );

    /**
     * @returns the 0'th input object of the filter.
     */
    const mitk::Image* GetInput();


    /**
    * @return possible file extensions for the data type associated with the writer
    */
    virtual std::vector<std::string> GetPossibleFileExtensions();

    std::string GetSupportedBaseData() const;


protected:

    /**
     * Constructor.
     */
    PicFileWriter();

    /**
     * Virtual destructor.
     */
    virtual ~PicFileWriter();

    virtual void GenerateData();

    virtual int MITKIpPicPut( char *outfile_name, mitkIpPicDescriptor *pic );

    std::string m_FileName;

    std::string m_FilePrefix;

    std::string m_FilePattern;
};

}

#endif //_MITK_PIC_FILE_WRITER__H_
