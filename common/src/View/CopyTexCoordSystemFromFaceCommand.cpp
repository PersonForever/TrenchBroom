/*
 Copyright (C) 2010-2017 Kristian Duske

 This file is part of TrenchBroom.

 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#include "CopyTexCoordSystemFromFaceCommand.h"

#include "TrenchBroom.h"
#include "Model/BrushFace.h"
#include "Model/Snapshot.h"
#include "View/MapDocumentCommandFacade.h"

#include <vecmath/plane.h>

#include <vector>

namespace TrenchBroom {
    namespace View {
        const Command::CommandType CopyTexCoordSystemFromFaceCommand::Type = Command::freeType();

        CopyTexCoordSystemFromFaceCommand::Ptr CopyTexCoordSystemFromFaceCommand::command(const Model::TexCoordSystemSnapshot& coordSystemSanpshot, const Model::BrushFaceAttributes& attribs, const vm::plane3& sourceFacePlane, const Model::WrapStyle wrapStyle) {
            return Ptr(new CopyTexCoordSystemFromFaceCommand(coordSystemSanpshot, attribs, sourceFacePlane, wrapStyle));
        }

        CopyTexCoordSystemFromFaceCommand::CopyTexCoordSystemFromFaceCommand(const Model::TexCoordSystemSnapshot& coordSystemSnapshot, const Model::BrushFaceAttributes& attribs, const vm::plane3& sourceFacePlane, const Model::WrapStyle wrapStyle) :
        DocumentCommand(Type, "Copy Texture Alignment"),
        m_snapshot(nullptr),
        m_coordSystemSanpshot(coordSystemSnapshot.clone()),
        m_sourceFacePlane(sourceFacePlane),
        m_wrapStyle(wrapStyle),
        m_attribs(attribs) {}

        CopyTexCoordSystemFromFaceCommand::~CopyTexCoordSystemFromFaceCommand() {
            delete m_snapshot;
            m_snapshot = nullptr;
        }

        bool CopyTexCoordSystemFromFaceCommand::doPerformDo(MapDocumentCommandFacade* document) {
            const std::vector<Model::BrushFace*> faces = document->allSelectedBrushFaces();
            assert(!faces.empty());

            assert(m_snapshot == nullptr);
            m_snapshot = new Model::Snapshot(std::begin(faces), std::end(faces));

            document->performCopyTexCoordSystemFromFace(*m_coordSystemSanpshot, m_attribs, m_sourceFacePlane, m_wrapStyle);
            return true;
        }

        bool CopyTexCoordSystemFromFaceCommand::doPerformUndo(MapDocumentCommandFacade* document) {
            document->restoreSnapshot(m_snapshot);
            delete m_snapshot;
            m_snapshot = nullptr;
            return true;
        }

        bool CopyTexCoordSystemFromFaceCommand::doIsRepeatable(MapDocumentCommandFacade* document) const {
            return document->hasSelectedBrushFaces();
        }

        UndoableCommand::Ptr CopyTexCoordSystemFromFaceCommand::doRepeat(MapDocumentCommandFacade*) const {
            return UndoableCommand::Ptr(new CopyTexCoordSystemFromFaceCommand(*m_coordSystemSanpshot, m_attribs, m_sourceFacePlane, m_wrapStyle));
        }

        bool CopyTexCoordSystemFromFaceCommand::doCollateWith(UndoableCommand::Ptr) {
            return false;
        }
    }
}
